#ifndef _DUST_TESTER_H_
#define _DUST_TESTER_H_

#include "c_hashing.h"

namespace dust
{
	class tester
	{
		typedef size_t NodeId;

		typedef c_hashing<NodeId> H;

		size_t N;

		H c1;
		H c2;

		void init(NodeId min, NodeId max)
		{
			c1 = c_hashing<size_t>();
			c2 = c_hashing<size_t>();

			for(NodeId i = 0; i < min; ++i)
				c1.insert_node(i);

			for(NodeId i = 0; i < max; ++i)
				c2.insert_node(i);

			c1.set_fault_tolerance(0.5);
			c2.set_fault_tolerance(0.5);
		}

		public:

		tester(size_t n) :
			N(n)
		{
		}

		void test_n_nodes(NodeId min, NodeId max)
		{
			init(min, max);

			c2.dump();

			for(size_t o = 0; o < N; ++o)
			{
				NodeId n2 = c2.get_node(o);
				if(n2 < min)
				{
					NodeId n1 = c1.get_node(o);
					if(n1 != n2)
						std::cout << "ERROR: object " << o << " is in node " << n1 << " for c1 but in node " << n2 << " for c2\n";
				}
			}

			c1.dump();
		}

		void test_diff(NodeId min, NodeId max)
		{
			c1 = c_hashing<size_t>();
			c2 = c_hashing<size_t>();

			for(NodeId i = 0; i < max; ++i)
			{
				c1.insert_node(i);
				c2.insert_node(i);
			}

			c1.set_fault_tolerance(0.5);
			c2.set_fault_tolerance(0.5);

			c1.dump();

			for(NodeId i = max; i > min; --i)
				c1.remove_node(i - 1);

			c1.dump();
		}

		void test_diff_vnodes(unsigned int min, unsigned int max)
		{
			init(min, max);

			c1.dump();
			c1.set_num_virtual_nodes(H::default_num_virtual_nodes / 2);
			c1.dump();
			c1.set_num_virtual_nodes(H::default_num_virtual_nodes);
			c1.dump();

		}

	};
}

#endif // _DUST_TESTER_H_
