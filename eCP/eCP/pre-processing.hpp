#pragma once
/**
 * @file distance.cpp
 * static functions for the eCP algorithm preprocessing phase
 *
 * @author Nikolaj Mertz
 * @author Frederik Martini
 * @date 15/5/2020
 */
#include "eCP.hpp"

 /*
  * contains static functions for the preprocessing phase of the eCP algorithm.
  *
  */
class Pre_Processing {
public:
	/**
	* creates the cluster index from dataset and level parameter L
	* @param dataset collection of all points to be placed in the created index
	* @param L levels of the index
	* @returns top_level nodes of the top level of the index
	*/
	static std::vector<Node*> create_index(std::vector<Point>& dataset, unsigned int L);

	/**
	* insert a collection of points into the cluster index
	* @param index_top_level index to insert points into
	* @param points collection of points to be inserted into the index
	* @param from_index index insertion starts from
	*/
	static std::vector<Node*> insert_points(std::vector<Node*>& index_top_level, std::vector<Point>& points, unsigned int from_index = 0);

	/**
	* find the nearest node to a query point
	* @param nodes vector of nodes to search through
	* @param query query point
	* @return pointer to the nearest node to the query point
	*/
	static Node* find_nearest_node(std::vector<Node*>& nodes, float*& query);
private:

	/**
	* recursively finds the nearest leaf to a query point from a level
	* @param query query point
	* @param node nearest node in current level
	* @param depth levels to go down
	* @return nearest leaf to query point
	*/
	static Node* find_nearest_leaf_from_level(float*& query, Node*& node, unsigned int depth);

	/*
	* generate vector of n values in range 0 to n
	*/
	static std::vector<unsigned> generate_random_numbers(const unsigned n, const unsigned range_from, const unsigned range_to);
	static std::vector<unsigned> random_vector_numbers(const unsigned n, std::vector<unsigned> numbers);

	static void shrink_node_children(std::vector<Node*>& nodes);
};