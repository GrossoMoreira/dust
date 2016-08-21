#include "dust/c_hashing.h"

#include <iostream>
#include <vector>

struct node
{
	int id;
	std::set<int> objects;
};

std::vector<node*> nodes;
dust::c_hashing<> H;

void dump_count()
{
	std::cout << "#Dump count:" << std::endl;
	for(node* n : nodes)
		std::cout << "n" << n->id << ": " << n->objects.size() << std::endl;
}

/* Creates n nodes */
void insert_nodes(int n)
{
	for(int i = 0; i < n; ++i)
	{
		node* n = new node();
		n->id = i;
		nodes.push_back(n);
		H.insert_node(i);
	}
}

/* Puts n objects in respective nodes */
void insert_objects(int n)
{
	for(int i = 0; i < n; ++i)
		nodes[H.get_node(i)]->objects.insert(i);
}

int main() {
	// create nodes and objects
	insert_nodes(10);
	insert_objects(1000000);

	// save which objects went to node 0
	std::set<int> sv0objects = nodes[0]->objects;

	// clear everything
	nodes.clear();
	H = dust::c_hashing<>();

	// now only insert half the nodes
	insert_nodes(5);

	// insert objects again
	insert_objects(1000000);

	// dump stuff
	dump_count();

	// check if the objects that were inserted in node 0 in the first attempt are also in node 0 now
	std::set<int> super = nodes[0]->objects;
	bool is_subset = std::includes(super.begin(), super.end(), sv0objects.begin(), sv0objects.end());

	if(is_subset)
		std::cout << "Great success! The most useful property of consistent hashing has been fulfilled!" << std::endl;
	else
		std::cout << "This implementation of consistent hashing has failed!" << std::endl;
}
