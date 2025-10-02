#include <require.hpp>
#include <iostream>

template<typename T = int64_t>
class Matrix
{
public:
    typedef T (*generator_t)(int i, int j);

private:
    int size;
    T *data;

public:
    Matrix(int size) : size(size)
    {
        data = new T[size*size];
        require(data, "Can't allocate memory for new matrix");
    }

    Matrix(int size, generator_t generate) : Matrix(size)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                data[i*size + j] = generate(i,j);
    }

    T *operator[](int i)
    {
        return data + i*size;
    }

    // not a good solution
    void free()
    {
        delete[] data;
        size = -1;
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

    static void multiply_n3(Matrix result, Matrix left, Matrix right)
    {
        require(result.size == left.size, "Inappropriate matrix sizes");
        require(left.size == right.size, "Inappropriate matrix sizes");
        int size = result.size;

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

    bool operator==(const Matrix &other)
    {
        if(other.size != size)
            return false;

        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                if(data[i*size+j] != other.data[i*size+j])
                    return false;

        return true;
    }

    bool operator!=(const Matrix &other)
    {
        return !(*this == other);
    }
};
