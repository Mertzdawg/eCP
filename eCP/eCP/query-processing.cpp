#include "eCP.hpp"
#include "query-processing.hpp"
#include "pre-processing.hpp"
#include "sort.hpp"

/*
 * recursively traverse the index to find the nearest leaf at the bottom level
 */
Node* Query_Processing::find_nearest_leaf(float*& query, std::vector<Node*>& nodes)
{
	Node* best_cluster = Pre_Processing::find_nearest_node(nodes, query);
	float closest = FLOAT_MAX;

	for (Node* cluster : best_cluster->children)
	{
		if (cluster->is_leaf())
		{
			return Pre_Processing::find_nearest_node(best_cluster->children, query);
		}

		const auto dist = g_distance_function(query, cluster->get_representative());
		if (dist < closest)
		{
			closest = dist;
			best_cluster = find_nearest_leaf(query, cluster->children);
		}
	}
	return best_cluster;
}

std::pair<std::vector<unsigned int>, std::vector<float>> Query_Processing::k_nearest_neighbors(std::vector<Node*>& root, float*& query, const unsigned int k, const unsigned int b, unsigned int L)
{
	//find b nearest clusters
	std::vector<Node*> b_nearest_clusters; //accumulator for b clusters
	b_nearest_clusters.reserve(b);
	b_nearest_clusters = find_b_nearest_clusters(root, query, b, L);

	//go through b clusters to obtain k nearest neighbors
	std::pair<std::vector<unsigned int>, std::vector<float>> k_nearest_points;
	for (Node* b_nearest_cluster : b_nearest_clusters)
	{
		scan_leaf_node(query, b_nearest_cluster->points, k, k_nearest_points);
	}

	std::pair<std::vector<unsigned int>, std::vector<float>> test;

	//sort by distance in ascending order
	Sort::sort(k_nearest_points);

	return k_nearest_points;
}

/*
 * traverses node children one level at a time to find b nearest
 */
std::vector<Node*> Query_Processing::find_b_nearest_clusters(std::vector<Node*>& root, float*& query, unsigned int b, unsigned int L)
{
	std::vector<Node*> b_best;
	b_best.reserve(b);
	scan_node(query, root, b, b_best);

	//if L > 1 go down index, if L == 1 simply return the b_best
	while (L > 1)
	{
		std::vector<Node*> new_best_nodes;
		new_best_nodes.reserve(b);
		for (auto* node : b_best)
		{
			scan_node(query, node->children, b, new_best_nodes);
		}
		L = L - 1;
		b_best = new_best_nodes;
	}

	return b_best;
}

/*
 * scans a node, adding the nearer nodes to an accumulator
 */
void Query_Processing::scan_node(float*& query, std::vector<Node*>& nodes, unsigned int& b, std::vector<Node*>& next_level_nodes)
{
	std::pair<int, float> furthest_node = std::make_pair(-1, -1);

	//if we already have enough nodes to start replacing, find the furthest node
	if (next_level_nodes.size() >= b) {
		furthest_node = find_furthest_node(query, next_level_nodes);
	}

	for (Node* node : nodes)
	{
		//not enough nodes yet, just add
		if (next_level_nodes.size() < b)
		{
			next_level_nodes.emplace_back(node);

			//next iteration we will start replacing, compute the furthest cluster
			if (next_level_nodes.size() == b) furthest_node = find_furthest_node(query, next_level_nodes);
		}
		else
		{
			//only replace if better
			if (g_distance_function(query, node->get_representative()) < furthest_node.second) {
				next_level_nodes[furthest_node.first] = node;

				//the furthest node has been replaced, find the new furthest
				furthest_node = find_furthest_node(query, next_level_nodes);
			}
		}
	}
}

/*
 * uses an accumulator nearest_points to store the result
 */
void Query_Processing::scan_leaf_node(float*& query, std::vector<Point>& points, const unsigned int k, std::pair<std::vector<unsigned int>, std::vector<float>>& nearest_points)
{
	std::pair<int, float> max = std::make_pair(-1, FLOAT_MAX);
	//if we already have enough points to start replacing, find the furthest point
	if (nearest_points.first.size() >= k) {
		unsigned int index = index_to_max_element(nearest_points);
		max = std::make_pair(nearest_points.first[index], nearest_points.second[index]);
	}

	for (Point& point : points)
	{
		//not enough points yet, just add
		if (nearest_points.first.size() < k)
		{
			float dist = g_distance_function(query, point.descriptor);

			nearest_points.first.emplace_back(point.id);
			nearest_points.second.emplace_back(dist);

			//next iteration we will start replacing, compute the furthest cluster
			if (nearest_points.first.size() == k) {
				unsigned int index = index_to_max_element(nearest_points);
				max = std::make_pair(nearest_points.first[index], nearest_points.second[index]);
			}
		}
		else
		{
			//only replace if nearer
			float dist = g_distance_function(query, point.descriptor);
			if (dist < max.second) {
				//replace furthest with new
				unsigned int index = index_to_max_element(nearest_points);
				nearest_points.first[index] = point.id;
				nearest_points.second[index] = dist;
					
				//the furthest point has been replaced, find the new furthest
				index = index_to_max_element(nearest_points);
				max = std::make_pair(nearest_points.first[index], nearest_points.second[index]);
			}
		}
	}
}

std::pair<int, float> Query_Processing::find_furthest_node(float*& query, std::vector<Node*>& nodes)
{
	std::pair<int, float> worst = std::make_pair(-1, -1);
	for (unsigned int i = 0; i < nodes.size(); i++) {
		const float dst = g_distance_function(query, nodes[i]->get_representative());

		if (dst > worst.second) {
			worst.first = i;
			worst.second = dst;
		}
	}

	return worst;
}

unsigned int Query_Processing::index_to_max_element(std::pair<std::vector<unsigned int>, std::vector<float>>& pairs)
{
	unsigned int index = -1;
	float min = FLOAT_MIN;

	for (unsigned int i = 0; i < pairs.second.size(); i++) {
		if (pairs.second[i] > min) {
			min = pairs.second[i];
			index = i;
		}
	}

	return index;
}