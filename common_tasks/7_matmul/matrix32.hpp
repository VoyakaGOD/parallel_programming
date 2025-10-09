#pragma once

#include <immintrin.h>
#include "matrix.hpp"

// wrapper for AVX512
// custom allocators for std::vector is evil: 0.108s -> 0.541s due to nothing
class Matrix32 : public Matrix<int32_t>
{
public:
    using Matrix::Matrix;

    Matrix32(const Matrix<int32_t> &source) : Matrix(source) {}

    static void multiply_n3_avx512(Matrix32 &result, const Matrix32 &left, const Matrix32 &right)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = result.size;
        const int32_t *left_data = left.get_data();
        const int32_t *right_data = right.get_data();
        int32_t *result_data = result.data.data();

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
                    __m512i left_vec = _mm512_loadu_epi32(left_data + i*size + k);

                    alignas(64) int32_t tmp[16];
                    for(int l = 0; l < 16; l++)
                        tmp[l] = right_data[(k+l)*size + j];
                    __m512i right_vec = _mm512_loadu_epi32(tmp);

                    __m512i product = _mm512_mullo_epi32(left_vec, right_vec);
                    block = _mm512_add_epi32(block, product);
                }
                alignas(64) int32_t vec[16];
                _mm512_store_epi32((__m512i*)vec, block);
                for(int l = 0; l < 16; l++)
                    sum += vec[l];

                for(; k < size; k++)
                    sum += left_data[i*size+k]*right_data[k*size+j];

                result_data[i*size+j] = sum;
            }
        }
    }

    static void multiply_n3_avx512_tr(Matrix32 &result, const Matrix32 &left, const Matrix32 &right_tr)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right_tr.size, "Inappropriate matrix sizes");
        int size = result.size;
        const int32_t *left_data = left.get_data();
        const int32_t *right_data = right_tr.get_data();
        int32_t *result_data = result.data.data();

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
                    __m512i left_vec = _mm512_loadu_epi32(left_data + i*size + k);

                    alignas(64) int32_t tmp[16];
                    for(int l = 0; l < 16; l++)
                        tmp[l] = right_data[j*size + (k+l)];
                    __m512i right_vec = _mm512_loadu_epi32(tmp);

                    __m512i product = _mm512_mullo_epi32(left_vec, right_vec);
                    block = _mm512_add_epi32(block, product);
                }
                alignas(64) int32_t vec[16];
                _mm512_store_epi32((__m512i*)vec, block);
                for(int l = 0; l < 16; l++)
                    sum += vec[l];

                for(; k < size; k++)
                    sum += left_data[i*size+k]*right_data[j*size+k];

                result_data[i*size+j] = sum;
            }
        }
    }

    static Matrix32 multiply_Strassen_avx512(const Matrix32 &left, const Matrix32 &right, int threshold = 16)
    {
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = left.size;
        require((size & (size - 1)) == 0, "Size should be power of 2");

        Matrix32 result(size);
        if(size < threshold)
        {
            multiply_n3_avx512(result, left, right);
            return result;
        }

        int half_size = size / 2;
        Matrix32 A11(half_size), A12(half_size), A21(half_size), A22(half_size);
        Matrix32 B11(half_size), B12(half_size), B21(half_size), B22(half_size);
        right.split(B11, B12, B21, B22);
        left.split(A11, A12, A21, A22);

        Matrix32 M1(0), M2(0), M3(0), M4(0), M5(0), M6(0), M7(0);
        #pragma omp parallel num_threads(omp_threads) if(omp_threads > 0)
        #pragma omp single
        {
            #pragma omp task
            M1 = multiply_Strassen_avx512(A11 + A22, B11 + B22, threshold);
            #pragma omp task
            M2 = multiply_Strassen_avx512(A21 + A22, B11, threshold);
            #pragma omp task
            M3 = multiply_Strassen_avx512(A11, B12 - B22, threshold);
            #pragma omp task
            M4 = multiply_Strassen_avx512(A22, B21 - B11, threshold);
            #pragma omp task
            M5 = multiply_Strassen_avx512(A11 + A12, B22, threshold);
            #pragma omp task
            M6 = multiply_Strassen_avx512(A21 - A11, B11 + B12, threshold);
            #pragma omp task
            M7 = multiply_Strassen_avx512(A12 - A22, B21 + B22, threshold);

            #pragma omp taskwait
        }

        result.unite(M1+M4+M7-M5, M3+M5, M2+M4, M1+M3+M6-M2);
        return result;
    }

    // Matrix32 operator+(const Matrix32 &other) const
    // {
    //     require(size == other.size, "Inappropriate matrix sizes");

    //     Matrix32 sum(size);
    //     int N = size*size;
    //     int n = 0;

    //     for(; n + 15 < N; n += 16)
    //     {
    //         __m512i left = _mm512_loadu_epi32(&data[n]);
    //         __m512i right = _mm512_loadu_epi32(&other.data[n]);
    //         __m512i block = _mm512_add_epi32(left, right);
    //         _mm512_storeu_epi32(&sum.data[n], block);
    //     }

    //     for (; n < N; n++)
    //         sum.data[n] = data[n] + other.data[n];

    //     return sum;
    // }

    // Matrix32 operator-(const Matrix32 &other) const
    // {
    //     require(size == other.size, "Inappropriate matrix sizes");

    //     Matrix32 diff(size);
    //     int N = size*size;
    //     int n = 0;

    //     for(; n + 15 < N; n += 16)
    //     {
    //         __m512i left = _mm512_loadu_epi32(&data[n]);
    //         __m512i right = _mm512_loadu_epi32(&other.data[n]);
    //         __m512i block = _mm512_sub_epi32(left, right);
    //         _mm512_storeu_epi32(&diff.data[n], block);
    //     }

    //     for (; n < N; n++)
    //         diff.data[n] = data[n] - other.data[n];

    //     return diff;
    // }
};
