#pragma once

#include <vector>

/**
 * @file sort.cpp
 * static functions for sorting results of leaves
 */

class Sort {
public:
    static void sort(std::pair<std::vector<unsigned int>, std::vector<float>>& v);

    static void quickSort(std::pair<std::vector<unsigned int>, std::vector<float>>& arr, int low, int high);
private:
    static void swap(unsigned int a, unsigned int b, std::pair<std::vector<unsigned int>, std::vector<float>>& arr);

    static int partition(std::pair<std::vector<unsigned int>, std::vector<float>>& arr, int low, int high);
};
