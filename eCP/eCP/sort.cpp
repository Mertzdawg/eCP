#include "sort.hpp"

//https://www.geeksforgeeks.org/cpp-program-for-quicksort/


void Sort::swap(unsigned int a, unsigned int b, std::pair<std::vector<unsigned int>, std::vector<float>>& arr)
{
    int tmp = arr.first[a];
    arr.first[a] = arr.first[b];
    arr.first[b] = tmp;

    tmp = arr.second[a];
    arr.second[a] = arr.second[b];
    arr.second[b] = tmp;
}

int Sort::partition(std::pair<std::vector<unsigned int>, std::vector<float>>& arr, int low, int high)
{
    int pivot = arr.second[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr.second[j] <= pivot)
        {
            i++;
            swap(i, j, arr);
        }
    }
    swap(i + 1, high, arr);
    return (i + 1);
}

//quicksort implementation modified to fit our data structure
void Sort::quickSort(std::pair<std::vector<unsigned int>, std::vector<float>>& arr, int low, int high)
{
    if (low < high)
    {
        int pi = Sort::partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

//main call to the sort function - easily replaced by other implementations
void Sort::sort(std::pair<std::vector<unsigned int>, std::vector<float>>& v) 
{
    Sort::quickSort(v, 0, v.first.size() -1);
}