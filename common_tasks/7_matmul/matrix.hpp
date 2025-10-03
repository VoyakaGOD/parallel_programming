#pragma once
#include <require.hpp>
#include <iostream>
#include <vector>
#include <omp.h>

template<typename T = int32_t>
class Matrix
{
public:
    typedef T (*generator_t)(int i, int j);

private:
    int size;
    std::vector<T> data;
    static int omp_threads;

    // no size check
    // only for size: 2|size
    void split(Matrix &M11, Matrix &M12, Matrix &M21, Matrix &M22) const
    {
        int half_size = size / 2;

        for(int i = 0; i < half_size; i++)
            for(int j = 0; j < half_size; j++)
                M11.data[i*half_size+j] = data[i*size+j];

        for(int i = 0; i < half_size; i++)
            for(int j = half_size; j < size; j++)
                M12.data[i*half_size+j-half_size] = data[i*size+j];

        for(int i = half_size; i < size; i++)
            for(int j = 0; j < half_size; j++)
                M21.data[(i-half_size)*half_size+j] = data[i*size+j];

        for(int i = half_size; i < size; i++)
            for(int j = half_size; j < size; j++)
                M22.data[(i-half_size)*half_size+j-half_size] = data[i*size+j];
    }

    // no size check
    // only for size: 2|size
    void unite(const Matrix &M11, const Matrix &M12, const Matrix &M21, const Matrix &M22)
    {
        int half_size = size / 2;

        for(int i = 0; i < half_size; i++)
            for(int j = 0; j < half_size; j++)
                data[i*size+j] = M11.data[i*half_size+j];

        for(int i = 0; i < half_size; i++)
            for(int j = half_size; j < size; j++)
                data[i*size+j] = M12.data[i*half_size+j-half_size];

        for(int i = half_size; i < size; i++)
            for(int j = 0; j < half_size; j++)
                data[i*size+j] = M21.data[(i-half_size)*half_size+j];

        for(int i = half_size; i < size; i++)
            for(int j = half_size; j < size; j++)
                data[i*size+j] = M22.data[(i-half_size)*half_size+j-half_size];
    }

public:
    // no omp by default, count = 0
    static void set_omp_threads(int count)
    {
        require(count >= 0, "Threads count should be non negative");
        omp_threads = count;
    }

    Matrix(int size) : size(size), data(size*size) {}

    Matrix(int size, generator_t generate) : Matrix(size)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                data[i*size + j] = generate(i,j);
    }

    T *operator[](int i)
    {
        return data.data() + i*size;
    }

    static Matrix new_identity_matrix(int size)
    {
        return Matrix(size, [](int i, int j)
        {
            return static_cast<T>((i == j) ? 1 : 0);
        });
    }

    void transpose()
    {
        T tmp;
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < i; j++)
            {
                tmp = data[j*size + i];
                data[j*size + i] = data[i*size + j];
                data[i*size + j] = tmp;
            }
        }
    }

    void transpose_block(int block_size)
    {
        require(size % block_size == 0, "Matrix size must be divisible by the block size");

        T tmp;
        for(int block_i = 0; block_i < size; block_i += block_size)
        {
            for(int block_j = 0; block_j <= block_i; block_j += block_size)
            {
                for(int i = block_i; i < block_i + block_size; i++)
                {
                    for(int j = block_j; j < block_j + block_size; j++)
                    {
                        if(j >= i)
                            break;
                        tmp = data[j*size + i];
                        data[j*size + i] = data[i*size + j];
                        data[i*size + j] = tmp;
                    }
                }
            }
        }
    }

    static void multiply_n3(Matrix result, Matrix left, Matrix right)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = result.size;

        #pragma omp parallel for collapse(2) num_threads(omp_threads) if(omp_threads > 0)
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
            {
                result.data[i*size+j] = 0;
                for(int k = 0; k < size; k++)
                    result.data[i*size+j] += left.data[i*size+k]*right.data[k*size+j];
            }
        }
    }

    static void multiply_n3_transpose(Matrix result, Matrix left, Matrix right_tr)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right_tr.size, "Inappropriate matrix sizes");
        int size = result.size;

        #pragma omp parallel for collapse(2) num_threads(omp_threads) if(omp_threads > 0)
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
            {
                result.data[i*size+j] = 0;
                for(int k = 0; k < size; k++)
                    result.data[i*size+j] += left.data[i*size+k]*right_tr.data[j*size+k];
            }
        }
    }

    static void multiply_n3_block(Matrix result, Matrix left, Matrix right, int block_size)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = result.size;
        require(size % block_size == 0, "Matrix size must be divisible by the block size");

        #pragma omp parallel for collapse(2) num_threads(omp_threads) \
        schedule(static) if(omp_threads > 0)
        for(int block_i = 0; block_i < size; block_i += block_size)
        {
            for(int block_j = 0; block_j < size; block_j += block_size)
            {
                for(int i = block_i; i < block_i + block_size; i++)
                    for(int j = block_j; j < block_j + block_size; j++)
                        result.data[i*size+j] = 0;

                for(int block_k = 0; block_k < size; block_k += block_size)
                    for(int i = block_i; i < block_i + block_size; i++)
                        for(int j = block_j; j < block_j + block_size; j++)
                            for(int k = block_k; k < block_k + block_size; k++)
                                result.data[i*size+j] += left.data[i*size+k]*right.data[k*size+j];
            }
        }
    }

    // block with transpose
    static void multiply_n3_bt(Matrix result, Matrix left, Matrix right_tr, int block_size)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right_tr.size, "Inappropriate matrix sizes");
        int size = result.size;
        require(size % block_size == 0, "Matrix size must be divisible by the block size");

        #pragma omp parallel for collapse(2) num_threads(omp_threads) \
        schedule(static) if(omp_threads > 0)
        for(int block_i = 0; block_i < size; block_i += block_size)
        {
            for(int block_j = 0; block_j < size; block_j += block_size)
            {
                for(int i = block_i; i < block_i + block_size; i++)
                    for(int j = block_j; j < block_j + block_size; j++)
                        result.data[i*size+j] = 0;

                for(int block_k = 0; block_k < size; block_k += block_size)
                    for(int i = block_i; i < block_i + block_size; i++)
                        for(int j = block_j; j < block_j + block_size; j++)
                            for(int k = block_k; k < block_k + block_size; k++)
                                result.data[i*size+j] += left.data[i*size+k]*right_tr.data[j*size+k];
            }
        }
    }

    static Matrix multiply_Strassen(Matrix left, Matrix right, int threshold = 16)
    {
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = left.size;
        require((size & (size - 1)) == 0, "Size should be power of 2");

        Matrix result(size);
        if(size < threshold)
        {
            multiply_n3(result, left, right);
            return result;
        }

        int half_size = size / 2;
        Matrix A11(half_size), A12(half_size), A21(half_size), A22(half_size);
        Matrix B11(half_size), B12(half_size), B21(half_size), B22(half_size);
        right.split(B11, B12, B21, B22);
        left.split(A11, A12, A21, A22);

        Matrix M1(0), M2(0), M3(0), M4(0), M5(0), M6(0), M7(0);
        #pragma omp parallel num_threads(omp_threads) if(omp_threads > 0)
        #pragma omp single
        {
            #pragma omp task
            M1 = multiply_Strassen(A11 + A22, B11 + B22, threshold);
            #pragma omp task
            M2 = multiply_Strassen(A21 + A22, B11, threshold);
            #pragma omp task
            M3 = multiply_Strassen(A11, B12 - B22, threshold);
            #pragma omp task
            M4 = multiply_Strassen(A22, B21 - B11, threshold);
            #pragma omp task
            M5 = multiply_Strassen(A11 + A12, B22, threshold);
            #pragma omp task
            M6 = multiply_Strassen(A21 - A11, B11 + B12, threshold);
            #pragma omp task
            M7 = multiply_Strassen(A12 - A22, B21 + B22, threshold);

            #pragma omp taskwait
        }

        result.unite(M1+M4+M7-M5, M3+M5, M2+M4, M1+M3+M6-M2);
        return result;
    }

    friend std::ostream &operator<<(std::ostream& out, const Matrix &matrix)
    {
        int size = matrix.size;

        for(int i = 0; i < size; i++)
        {
            out << matrix.data[i*size];
            for(int j = 1; j < size; j++)
                out << ", " << matrix.data[i*size + j];
            out << std::endl << std::endl;
        }

        return out;
    }

    bool operator==(const Matrix &other) const
    {
        return data == other.data;
    }

    bool operator!=(const Matrix &other) const
    {
        return data != other.data;
    }

    Matrix operator+(const Matrix &other) const
    {
        require(size == other.size, "Inappropriate matrix sizes");

        Matrix sum(size);
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                sum.data[i*size+j] = data[i*size+j] + other.data[i*size+j];

        return sum;
    }

    Matrix operator-(const Matrix &other) const
    {
        require(size == other.size, "Inappropriate matrix sizes");

        Matrix diff(size);
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                diff.data[i*size+j] = data[i*size+j] - other.data[i*size+j];

        return diff;
    }
};

template<typename T>
int Matrix<T>::omp_threads = 0;
