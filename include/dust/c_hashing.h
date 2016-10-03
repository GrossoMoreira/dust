#ifndef _DUST_C_HASHING_
#define _DUST_C_HASHING_

#include "generic_hash.h"
#include "transfer_plan.h"
#include "exceptions.h"

#include <cmath>
#include <set>
#include <vector>
#include <list>
#include <deque>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>

namespace dust
{
	template <typename NodeId = size_t, typename Key = size_t>
	class c_hashing
	{

		typedef unsigned int vnode_id;

		struct vnode
		{
			NodeId node;
			vnode_id vnode;
		};

		struct slice
		{
			hash position;
			NodeId node;
			vnode_id vnode_n;
			slice* prev; // the next slice of a virtual node with equal vnode_n
			slice* next; // the next slice of a virtual node with equal vnode_n
			typename std::list<slice>::iterator it;

			slice(hash pos, NodeId node, vnode_id vn) :
				position(pos),
				node(node),
				vnode_n(vn),
				prev(nullptr),
				next(nullptr)
			{
			}

			std::string str(vnode_id nv) const
			{
				std::stringstream ss;
				ss << node << " #" << vnode_n << " [ ";

				slice* s = next;

				while(nv)
				{
					ss << s->node << " #" << s->vnode_n << ", ";
					s = s->next;
					--nv;
				}

				ss << "] ~ " << position.str();
				return ss.str();
			}

			std::string str_id() const
			{
				std::stringstream ss;
				ss << node << " #" << vnode_n;
				return ss.str();
			}
		};

		vnode_id num_virtual_nodes;
		double fault_tolerance;
		vnode_id backup_range;

		std::set<NodeId> nodes;
		std::list<slice> slices;

		typedef transfer_plan<NodeId> plan;
		typedef typename std::list<slice>::iterator iterator;

		plan insert_vnode(const NodeId& node, vnode_id v_n)
		{
			plan transfers;

			hash h(node, v_n);

			auto it = slices.begin();

			auto slice_before = slices.end();
			for(auto et = slices.end(); it != et; ++it)
			{
				if(h < it->position)
					break;

				slice_before = it;
			}

			if(slice_before == slices.end())
				slice_before = slices.begin();

			auto inserted = slices.emplace(it, h, node, v_n); // insert new slice
			inserted->it = inserted;

			//std::cout << "slice before: " << slice_before->position.str() << std::endl;
			//std::cout << "slice inserted: " << inserted->position.str() << std::endl;

			auto next_slice = inserted;
			++next_slice;
			if(next_slice == slices.end())
				next_slice = slices.begin();

			if(nodes.size() != 1 && slice_before->node != node)
				transfers.plan(h, next_slice->position, slice_before->node, node, plan::SPLIT);

			auto forward = inserted;
			slice* current = &(*inserted);

			// find next slice with same vnode_n
			do
			{
				++forward;
				if(forward == slices.end())
					forward = slices.begin();
			}
			while(forward->vnode_n != v_n);

			if(forward->node == node) // if we only found the inserted vnode itself, then there are no others
			{
				current->next = current;
				current->prev= current;
			}
			else
			{
				slice* next = &(*forward);
				slice* prev = next->prev;

				inserted->next = next;
				inserted->prev = prev;
				prev->next = current;
				next->prev = current;
			}

			return transfers;
		}

		plan remove_vnode(iterator previous, iterator& remove)
		{
			plan transfers;

			//std::cout << "slice previous: " << previous->position.str() << std::endl;
			//std::cout << "slice remove: " << remove->position.str() << std::endl;

			// re-link previous and next v cousins
			slice* next = remove->next;
			slice* prev = remove->prev;

			prev->next = next;
			next->prev = prev;

			// traverse v cousins to the last
			for(unsigned int i = 1; i < backup_range; ++i)
				next = next->next;

			// data transfers for backups
			for(unsigned int i = 0; i < backup_range; ++i)
			{
				transfers.plan(next->position, next->next->position, remove->node, prev->node, plan::BACKUP);
				next = next->prev;
				prev = prev->prev;
			}

			while(previous->node == remove->node)
			{
				if(previous == slices.begin())
					previous = slices.end();

				--previous;
			}

			hash range_from = remove->position;
			NodeId node_from = remove->node;

			remove = slices.erase(remove);
			if(remove == slices.end())
				remove = slices.begin();

			hash range_to = remove->position;

			// data transfer of slice
			transfers.plan(range_from, range_to, node_from, previous->node, plan::JOIN);

			return transfers;
		}

		public:

		static constexpr vnode_id default_num_virtual_nodes = 3;

		c_hashing(vnode_id num_virtual_nodes = default_num_virtual_nodes) :
			num_virtual_nodes(num_virtual_nodes),
			fault_tolerance(0),
			backup_range(0)
		{
		}

		c_hashing(const c_hashing& other)
		{
			num_virtual_nodes = other.num_virtual_nodes;
			fault_tolerance = other.fault_tolerance;
			backup_range = other.backup_range;

			for(NodeId n : other.nodes)
				insert_node(n);
		}

		void dump()
		{
			std::cout << "dumping c_hashing table\n";
			for(auto& p : slices)
			{
				std::cout << p.str(backup_range) << std::endl;
			}
		}

		void set_fault_tolerance(double tolerance)
		{
			if(tolerance < 0 || tolerance > 1)
				return;

			decltype(backup_range) new_range = std::ceil(tolerance * nodes.size());

			plan transfers;

			if(new_range > backup_range)
			{
				for(slice& s : slices)
				{
					std::cout << "current slice: " << s.str_id() << std::endl;

					slice* next = s.next;
					auto count = backup_range;
					while(count--)
						next = next->next;

					std::cout << "next: " << next->str_id() << std::endl;

					for(auto r = backup_range; r < new_range; ++r)
					{
						auto range = next->it;
						++range;
						if(range == slices.end())
							range = slices.begin();

						transfers.plan(next->position, range->position, next->node, s.node, plan::BACKUP);
						next = next->next;
					}
				}
			}
			else if(new_range < backup_range)
			{
				for(slice& s : slices)
				{
					slice* next = s.next;
					auto count = new_range;
					while(count--)
						next = next->next;

					auto diff = backup_range - new_range;
					while(diff--)
					{
						auto range = next->it;
						++range;
						if(range == slices.end())
							range = slices.begin();
			
						transfers.plan(next->position, range->position, next->node, next->node, plan::NEGLECT);
						next = next->next;
					}
				}
			}

			backup_range = new_range;
			std::cout << "fault tolerance set to " << backup_range << " (" << tolerance * 100 << "%)\n";

			transfers.dump();
		}

		plan set_num_virtual_nodes(unsigned int n)
		{
			if(n == 0)
				throw invalid_argument("set_num_virtual_nodes requires n > 0");

			plan transfers;

			if(n < num_virtual_nodes)
			{
				auto prev = slices.end();
				--prev;

				for(auto it = slices.begin(), et = slices.end(); it != et;)
				{
					if(it->vnode_n >= n)
						transfers.merge(remove_vnode(prev, it));
					else
					{
						++it;
						++prev;

						if(prev == slices.end())
							prev = slices.begin();
					}
				}
			}

			if(n > num_virtual_nodes)
			{
				for(const NodeId& node : nodes)
					for(vnode_id i = num_virtual_nodes; i < n; ++i)
						transfers.merge(insert_vnode(node, i));
			}

			num_virtual_nodes = n;

			return transfers;
		}

		double get_fault_tolerance()
		{
			return fault_tolerance;
		}

		void insert_node(NodeId node)
		{
			if(nodes.find(node) != nodes.end())
				return;

			nodes.insert(node);

			plan p;

			for(vnode_id i = 0; i < num_virtual_nodes; ++i)
				p.merge(insert_vnode(node, i));

			p.dump();
		}

		plan remove_node(NodeId node)
		{
			std::cout << "remove_node\n";
			plan transfers;

			auto find = nodes.find(node);
			if(find == nodes.end())
				return transfers;

			nodes.erase(find);

			if(nodes.empty())
			{
				slices.clear();
				return transfers;
			}

			auto previous = slices.end();
			--previous;

			std::cout << "previous starts at " << previous->position.str() << std::endl;

			for(auto it = slices.begin(), et = slices.end(); it != et;)
			{
				if(it->node == node)
					transfers.merge(remove_vnode(previous, it));
				else
				{
					++it;
					++previous;

					if(previous == slices.end())
						previous = slices.begin();
				}
			}

			transfers.dump();

			return transfers;
		}

		NodeId get_node(Key k)
		{
			hash h(k);

			auto it = slices.begin();

			auto a = slices.rbegin();
			NodeId n = a->node;

			for(auto et = slices.end(); it != et; ++it)
				if(h < (*it).position)
					break;
				else
					n = it->node;

			return n;
		}
	};
}

#endif // _DUST_C_HASHING_
