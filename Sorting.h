#pragma once

#include <vector>
#include <tuple>
#include <stddef.h>
#include <cstdint>
#include <iostream>
#include <cassert>
#include <math.h>

template <typename T>
class QuickSort
{
private:
    
    size_t partition(size_t low, size_t high) { //high is location of pivot --- returns pivot position
        auto pivot = arr[high];

        size_t i = low;

        for (size_t j = 1; j < high; j++)
        {
            if (arr[j] < pivot) {
                std::swap(arr[i], arr[j]);
                i++;
            }
        }

        std::swap(arr[i], arr[high]);
        
        return i;
    }

    void rec(size_t low, size_t high) { 
        if (low >= high) return; //base case

        //recursive case

        //Median of 3
        const size_t middle = low + (high-low)/2;
        auto p1 = arr[low];
        auto p2 = arr[middle];
        auto p3 = arr[high];

        auto _min = std::min(p1, min(p2, p3));
        auto _max = std::max(p1, max(p2, p3));
        auto _med = (p1 + p2 + p3) - _min - _max;

        size_t med_pos = (_med == p1 ? low : (_med == p2 ? middle : high));

        if (med_pos != high)
            std::swap(arr[med_pos], arr[high]);

        auto pi_pos = partition(low, high);

        rec(low, pi_pos-1);
        rec(pi_pos+1, high);
    }

    std::vector<T> &arr;
public:
    QuickSort(std::vector<T> &arr) : arr(arr) {

    }
    ~QuickSort();

    void sort() {

    };
};

bool test_quicksort() {
    
};