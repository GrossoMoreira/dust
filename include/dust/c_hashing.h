#ifndef _DUST_C_HASHING_
#define _DUST_C_HASHING_

#include "generic_hash.h"

#include <set>
#include <vector>
#include <utility>
#include <iostream>

namespace dust
{
	template <typename NodeId = int, typename Key = int>
	class c_hashing
	{
		static const unsigned int num_virtual_nodes = 20;

		struct vnode
		{
			NodeId node;
			unsigned int vnode;
		};

		std::set<NodeId> nodes;
		std::vector<std::pair<NodeId, hash>> slices;

		public:

		c_hashing() {
		}

		void dump()
		{
			std::cout << "dumping c_hashing table\n";
			for(auto& p : slices)
				std::cout << p.first << " # " << p.second.str() << std::endl;
		}

		void insert_node(NodeId node)
		{
			if(nodes.find(node) != nodes.end())
				return;

			nodes.insert(node);

			for(unsigned int i = 0; i < num_virtual_nodes; ++i)
			{
				vnode v = { node, i };

				hash h(v);

				std::pair<int, hash> range(node, h);

				auto it = slices.begin();

				for(auto et = slices.end(); it != et; ++it)
					if(h < (*it).second)
						break;

				slices.insert(it, range);
			}
		}

		void remove_node(NodeId node)
		{
			auto it = nodes.find(node);
			if(it == nodes.end())
				return;

			nodes.erase(it);

			for(auto it = slices.begin(), et = slices.end(); it != et;)
				if((*it).first == node)
					it = slices.erase(it);
				else
					++it;
		}

		int get_node(Key k)
		{
			hash h(k);

			auto it = slices.begin();

			for(auto et = slices.end(); it != et; ++it)
				if(h < (*it).second)
					break;

			if(it == slices.begin())
				return (*slices.end()).first;

			return (*(it-1)).first;
		}
	};
}

#endif // _DUST_C_HASHING_
