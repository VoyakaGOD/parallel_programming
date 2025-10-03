#pragma once

#include <immintrin.h>
#include "matrix.hpp"

// wrapper for AVX512
// aligned data
class Matrix32 : public Matrix<int32_t>
{
public:
    using Matrix::Matrix; // same constructors, todo: align

    Matrix32(const Matrix &source) : Matrix(source) {}

    static void multiply_n3_avx512(Matrix32 result, Matrix32 left, Matrix32 right)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = result.size;

        #pragma omp parallel for collapse(2) num_threads(omp_threads) if(omp_threads > 0)
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
            {
                __m512i block = _mm512_setzero_si512();
                int32_t sum = 0;
                int k = 0;

                for(; k + 15 < size; k += 16)
                {
                    __m512i left_vec = _mm512_loadu_epi32(&left.data[i*size + k]);

                    alignas(64) int32_t tmp[16];
                    for(int l = 0; l < 16; l++)
                        tmp[l] = right.data[(k+l)*size + j];
                    __m512i right_vec = _mm512_loadu_epi32(tmp);

                    __m512i product = _mm512_mullo_epi32(left_vec, right_vec);
                    block = _mm512_add_epi32(block, product);
                }
                alignas(32) int32_t vec[16];
                _mm512_store_epi32((__m512i*)vec, block);
                for(int l = 0; l < 16; l++)
                    sum += vec[l];

                for(; k < size; k++)
                    sum += left.data[i*size+k]*right.data[k*size+j];

                result.data[i*size+j] = sum;
            }
        }
    }

    Matrix32 operator+(const Matrix32 &other) const
    {
        require(size == other.size, "Inappropriate matrix sizes");

        Matrix32 sum(size);
        int N = size*size;
        int n = 0;

        for(; n + 15 < N; n += 16)
        {
            __m512i left = _mm512_loadu_epi32(&data[n]);
            __m512i right = _mm512_loadu_epi32(&other.data[n]);
            __m512i block = _mm512_add_epi32(left, right);
            _mm512_storeu_epi32(&sum.data[n], block);
        }

        for (; n < N; n++)
            sum.data[n] = data[n] + other.data[n];

        return sum;
    }

    Matrix32 operator-(const Matrix32 &other) const
    {
        require(size == other.size, "Inappropriate matrix sizes");

        Matrix32 diff(size);
        int N = size*size;
        int n = 0;

        for(; n + 15 < N; n += 16)
        {
            __m512i left = _mm512_loadu_epi32(&data[n]);
            __m512i right = _mm512_loadu_epi32(&other.data[n]);
            __m512i block = _mm512_sub_epi32(left, right);
            _mm512_storeu_epi32(&diff.data[n], block);
        }

        for (; n < N; n++)
            diff.data[n] = data[n] - other.data[n];

        return diff;
    }
};
