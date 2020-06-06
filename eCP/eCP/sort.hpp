#pragma once

#include <vector>
#include <algorithm>

/**
 * @file sort.cpp
 * static functions for sorting results of leaves
 */

class Sort {
public:
    static void sort(std::pair<std::vector<unsigned int>, std::vector<float>>& v);

    static void quickSort(std::pair<std::vector<unsigned int>, std::vector<float>>& arr, int low, int high);
private:
    static int partition(std::pair<std::vector<unsigned int>, std::vector<float>>& arr, int low, int high);
};
