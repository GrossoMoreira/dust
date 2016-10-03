#include "dust/c_hashing.h"
#include "dust/cache.h"
#include "dust/tester.h"

#include <shpp/shpp.h>

#include <iostream>
#include <vector>
#include <functional>

typedef size_t NodeId;
typedef size_t ObjId;

void test_cache()
{

	struct asd
	{
		char a[30];
	};

	std::cout << "test_cache\n";
	dust::cache<> C(20);

	for(int i = 0; i < 10; ++i)
	{
		long* l = new long[1];
		C.put(i, l); 
	}


	C.put(10, new asd[1]);
}



struct node
{
	NodeId id;
	std::set<ObjId> objects;
};

std::vector<node*> nodes;
dust::c_hashing<> H;

void dump_count()
{
	std::cout << "#Dump count:" << std::endl;
	for(node* n : nodes)
		std::cout << "n" << n->id << ": " << n->objects.size() << std::endl;
}

void dump_objects()
{
	std::cout << "#Dump objects:" << std::endl;
	for(node* n : nodes)
	{
		std::cout << "n" << n->id << ": " << n->objects.size() << " :\t[ ";
		for(ObjId o : n->objects)
		{
			std::cout << o << " ";
		}
		std::cout << std::endl;
	}
}

/* Creates n nodes */
void insert_nodes(dust::c_hashing<>& H, NodeId n)
{
	for(NodeId i = 0; i < n; ++i)
	{
		node* n = new node();
		n->id = i;
		nodes.push_back(n);
		H.insert_node(i);
	}
}

void clear_nodes()
{
	for(node* n : nodes)
		delete n;
	nodes.clear();
}

/* Puts n objects in respective nodes */
void insert_objects(ObjId n)
{
	for(ObjId i = 0; i < n; ++i)
	{
		NodeId target = H.get_node(i);
		if(target > nodes.size() - 1)
		{
			std::cout << "insert " << i << " into " << target << " out of bounds (" << nodes.size() << ")\n";
			exit(-1);
		}
		nodes[target]->objects.insert(i);
	}
}

void test_c_hash()
{
	insert_nodes(H, 4);
	H.set_fault_tolerance(0.5);
	H.dump();

	dust::c_hashing<> H2;
	insert_nodes(H2, 3);
	H2.set_fault_tolerance(0.5);

	H2.dump();

}

dust::c_hashing<> h;

int main() {

	ObjId O = 1000;
	NodeId N = 10;

	shpp::service s;

	s.provide("add", +[](int a) { h.insert_node(static_cast<size_t>(a)); });
	s.provide("rangeadd", +[](int a, int b) { for(int i = a; i <= b; ++i) h.insert_node(static_cast<size_t>(i)); });
	s.provide("rm", +[](int a) { h.remove_node(static_cast<size_t>(a)); });
	s.provide("tolerance", +[](double t) { h.set_fault_tolerance(t); });
	s.provide("vnodes", +[](int n) { h.set_num_virtual_nodes(static_cast<unsigned int>(n)); });
	s.provide("dump", +[]() { h.dump(); });

	shpp::shell sh(s, shpp::shell::colors_disabled);
	sh.start();
}
