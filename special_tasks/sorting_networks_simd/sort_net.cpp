#include <require.hpp>
#include <immintrin.h>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>

enum class dir_t
{
    Ascending = true,
    Descending = false
};

typedef int32_t value_t;

namespace classic
{
    void compare_and_swap(value_t *array, int left, int right, dir_t direction)
    {
        if (static_cast<bool>(direction) == (array[left] > array[right]))
        {
            value_t tmp = array[left];
            array[left] = array[right];
            array[right] = tmp;
        }
    }

    void bitonic_merge(value_t *array, int left, int size, dir_t direction)
    {
        if (size > 1)
        {
            int half_size = size / 2;

            for (int i = left; i < left + half_size; i++)
                compare_and_swap(array, i, i + half_size, direction);

            bitonic_merge(array, left, half_size, direction);
            bitonic_merge(array, left + half_size, half_size, direction);
        }
    }

    void bitonic_sort(value_t *array, int left, int size, dir_t direction)
    {
        if (size > 1)
        {
            int half_size = size / 2;

            bitonic_sort(array, left, half_size, dir_t::Ascending);
            bitonic_sort(array, left + half_size, half_size, dir_t::Descending);
            bitonic_merge(array, left, size, direction);
        }
    }

    void bitonic_sort(value_t *array, int size, dir_t direction = dir_t::Ascending)
    {
        bitonic_sort(array, 0, size, direction);
    }
}

namespace simd
{
    void compare_and_swap(value_t* left, value_t* right, dir_t direction)
    {
        __m512i left_value = _mm512_load_epi32(left);
        __m512i right_value = _mm512_load_epi32(right);

        __m512i min_value = _mm512_min_epi32(left_value, right_value);
        __m512i max_value = _mm512_max_epi32(left_value, right_value);

        if (direction == dir_t::Ascending)
        {
            _mm512_store_epi32(left, min_value);
            _mm512_store_epi32(right, max_value);
        }
        else
        {
            _mm512_store_epi32(left, max_value);
            _mm512_store_epi32(right, min_value);
        }
    }

    void insertion_sort(value_t *array, int left, int right, dir_t direction)
    {
        for(int i = left + 1; i < right; i++)
        {
            value_t pivot = array[i];
            int j = i - 1;
            while((j >= left) && (static_cast<bool>(direction) == (array[j] > pivot)))
            {
                array[j+1] = array[j];
                j--;
            }
            array[j+1] = pivot;
        }
    }

    void bitonic_merge(value_t *array, int left, int size, dir_t direction)
    {
        int half_size = size / 2;

        if (size > 16)
        {
            for (int i = left; i < left + half_size; i += 16)
                compare_and_swap(array + i, array + i + half_size, direction);

            bitonic_merge(array, left, half_size, direction);
            bitonic_merge(array, left + half_size, half_size, direction);
        }
        else
        {
            insertion_sort(array, left, left + size, direction);
        }
    }

    void bitonic_sort(value_t *array, int left, int size, dir_t direction)
    {
        if (size > 16)
        {
            int half_size = size / 2;

            bitonic_sort(array, left, half_size, dir_t::Ascending);
            bitonic_sort(array, left + half_size, half_size, dir_t::Descending);
            bitonic_merge(array, left, size, direction);
        }
        else
        {
            insertion_sort(array, left, left + size, direction);
        }
    }

    void bitonic_sort(value_t *array, int size, dir_t direction = dir_t::Ascending)
    {
        bitonic_sort(array, 0, size, direction);
    }
}

bool is_sorted(value_t *array, int size, dir_t direction = dir_t::Ascending)
{
    for(int i = 1; i < size; i++)
    {
        if(array[i-1] == array[i])
            continue;
        if(static_cast<bool>(direction) == (array[i-1] > array[i]))
            return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    require(argc > 1, "You should specify array size[log_2(N)]");
    int logN = std::stoi(argv[1]);
    require(logN > 0, "Array size[log_2(N)] should be positive");
    int N = 1 << logN;
    std::cout << "Array size: " << N << std::endl;

    std::random_device random_dev;
    std::mt19937 engine(random_dev());
    std::uniform_int_distribution<value_t> dist(INT32_MIN, INT32_MAX);

    value_t *array;
    posix_memalign((void**)&array, 64, sizeof(value_t) * N);
    require(array, "Can't allocate memory for array");
    for (int i = 0; i < N; ++i)
        array[i] = dist(engine);

    auto start = std::chrono::high_resolution_clock::now();
    classic::bitonic_sort(array, N);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "classic bitonic time: " << elapsed.count();
    std::cout << ", " << (is_sorted(array, N) ? "V" : "X") << std::endl;

    for (int i = 0; i < N; ++i)
        array[i] = dist(engine);

    start = std::chrono::high_resolution_clock::now();
    std::sort(array, array + N);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "std::sort time: " << elapsed.count();
    std::cout << ", " << (is_sorted(array, N) ? "V" : "X") << std::endl;

    for (int i = 0; i < N; ++i)
        array[i] = dist(engine);

    start = std::chrono::high_resolution_clock::now();
    simd::bitonic_sort(array, N);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "simd bitonic time: " << elapsed.count();
    std::cout << ", " << (is_sorted(array, N) ? "V" : "X") << std::endl;
}
