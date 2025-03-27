// radix sort with base 16

#ifndef SPECIAL_RADIX_SORT
#define SPECIAL_RADIX_SORT

#include <mpi_try.hpp>
#include <cstring>

class RadixSorter
{
private:
    static uint64_t extract_digit(uint64_t value, int digit_index)
    {
        return (value >> (4 * digit_index)) % 16;
    }

    static void counting_sort(uint64_t *src, uint64_t *dst, int size, int digit_index)
    {
        int count[16] = {};
        for(int i = 0; i < size; i++)
            count[extract_digit(src[i], digit_index)]++;

        for(int i = 1; i < 16; i++)
            count[i] += count[i - 1];

        for(int i = size - 1; i >= 0; i--)
        {
            uint64_t digit = extract_digit(src[i], digit_index);
            dst[--count[digit]] = src[i];
        }
    }

public:
    static void sort(uint64_t *array, int size, int digits_count)
    {
        uint64_t *buffer = new uint64_t[size];
        TRY(buffer == nullptr, "Can't allocate memory for radix sort");

        for(int digit_index = 0; digit_index < digits_count; digit_index++)
        {
            if(digit_index & 0x1)
                counting_sort(buffer, array, size, digit_index);
            else
                counting_sort(array, buffer, size, digit_index);
        }

        if(digits_count & 0x1)
            std::memcpy(array, buffer, size * sizeof(uint64_t));

        delete[] buffer;
    }
};

#endif //SPECIAL_RADIX_SORT
