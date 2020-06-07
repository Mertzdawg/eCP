#include "eCP.hpp"
#include "pre-processing.hpp"
#include "query-processing.hpp"

/**
 * the data set is partitioned bottom up and the levels connected top down from the top level.
 */
std::vector<Node*> Pre_Processing::create_index(std::vector<Point>& dataset, unsigned int L)
{
	//get level size for each level
	std::vector<unsigned int> level_sizes;
	level_sizes.reserve(L);

	const auto initial_l = ceil(pow(dataset.size(), ((L / (L + 1.00)))));
	level_sizes.push_back(initial_l);

	for (unsigned int i = 1; i < L; ++i)
	{
		const auto level_l = ceil(pow(level_sizes[i - 1], (((L - i) / (L + 1.00)))));
		level_sizes.push_back(level_l);
	}
	level_sizes.shrink_to_fit();
	
	//generate random indexes
	std::vector<std::vector<unsigned int>> level_indexes;
	level_indexes.reserve(L);
	std::vector<unsigned int>* curr_partition = nullptr;
	for (auto && size : level_sizes)
	{
		//bottom level
		if (curr_partition == nullptr)
		{
			level_indexes.push_back(generate_random_numbers(size, 0, dataset.size()));
			curr_partition = &level_indexes[0];
		} else
		{
			level_indexes.push_back(random_vector_numbers(size,*curr_partition));
			curr_partition = &level_indexes[level_indexes.size() - 1];
		}
	}
	//reverse level list for increasing level numbering
	reverse(level_indexes.begin(), level_indexes.end());
	
	//build top level, using first n^(1/(L+1)) points of data set
	std::vector<Node*> top_level;
	
	top_level.reserve(level_sizes[level_sizes.size() - 1]);
	for (auto && top_level_leader : level_indexes[0])
	{
		top_level.emplace_back(new Node(dataset[top_level_leader]));
	}
	top_level.shrink_to_fit();
	
	//insert each level of clusters to form index - skip already built top level
	for (unsigned int level = 1; level < L; ++level)
	{
		//go through each level
		for (unsigned int leader_index : level_indexes[level])
		{
			auto* upper_level_nearest = find_nearest_node(top_level, dataset[leader_index].descriptor);
			// -1 since top level has been compared with
			auto* lower_level_nearest = find_nearest_leaf_from_level(dataset[leader_index].descriptor, upper_level_nearest, level - 1);

			const unsigned int avg_rep = ceil(pow(dataset.size(), (1.00 / (L + 1.00))));
			//at bottom level
			if (level == L - 1)
			{
				auto* leaf = new Node(dataset[leader_index]);
				//each leaf cluster always represents, on average, n^( 1/(L+1) ) point
				leaf->points.reserve(avg_rep);
				lower_level_nearest->children.emplace_back(leaf);
			}
			else
			{
				auto* node = new Node(dataset[leader_index]);
				node->children.reserve(avg_rep);
				lower_level_nearest->children.emplace_back(node);
			}
		}
	}

	return top_level;
}

std::vector<unsigned int> Pre_Processing::generate_random_numbers(const unsigned int n, const unsigned int range_from, const unsigned int range_to)
{
	std::vector<unsigned int> values(range_to - range_from);

	// values now has range_from to range_to
	std::generate(values.begin(), values.end(), [value = range_from]() mutable { return value++; });

	// obtain random order
	std::shuffle(values.begin(), values.end(), std::mt19937{ std::random_device{}() });

	//now we have the n random numbers
	values.resize(n);
	
	return values;
}

std::vector<unsigned int> Pre_Processing::random_vector_numbers(const unsigned int n, std::vector<unsigned int> numbers)
{
	// obtain random order
	std::shuffle(numbers.begin(), numbers.end(), std::mt19937{ std::random_device{}() });

	//now we have the n random numbers
	numbers.resize(n);

	return numbers;
}

void Pre_Processing::shrink_node_children(std::vector<Node*>& nodes)
{
	if (nodes[0]->is_leaf()) {
		for (Node* n : nodes)
		{
			n->points.shrink_to_fit();
		}
		//at bottom
		return;
	}
	
	for (Node* n : nodes)
	{
		n->children.shrink_to_fit();
		shrink_node_children(n->children);
	}
}

std::vector<Node*> Pre_Processing::insert_points(std::vector<Node*>& index_top_level, std::vector<Point>& points, unsigned int from_index)
{
	for (unsigned int i = from_index; i < points.size(); ++i)
	{
		Node* nearest = Query_Processing::find_nearest_leaf(points[i].descriptor, index_top_level);

		nearest->points.push_back(points[i]);
	}

	shrink_node_children(index_top_level);
	
	return index_top_level;
}

Node* Pre_Processing::find_nearest_node(std::vector<Node*>& nodes, float*& query)
{
	float nearest = FLOAT_MAX;
	Node* best = nullptr;
	//compare distance to every representative
	for (auto* node : nodes)
	{
		const float distance = g_distance_function(query, node->representative);
		if (distance <= nearest)
		{
			nearest = distance;
			best = node;
		}
	}
	return best;
}

/*
 * only used during index creation. Current level is required since index is still being built
 */
Node* Pre_Processing::find_nearest_leaf_from_level(float*& query, Node*& node, unsigned int depth)
{
	//If at bottom level
	if (depth == 0)
	{
		return node;
	}

	//continue down to next level
	Node* nearest = find_nearest_node(node->children, query);
	return find_nearest_leaf_from_level(query, nearest, depth - 1);
}