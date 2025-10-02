#include "matrix.hpp"
#include <omp.h>

int main()
{
    int size = 500;
    Matrix<> A = Matrix<>::new_identity_matrix(size);
    A[3][4] = 7;
    Matrix<> B = Matrix<>::new_identity_matrix(size);
    B[2][3] = 5;
    Matrix<> reference(size);
    Matrix<> C(size);

    double start = omp_get_wtime();
    Matrix<>::multiply_n3(reference, A, B);
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    B.transpose();
    start = omp_get_wtime();
    Matrix<>::multiply_n3_transpose(C, A, B);
    std::cout << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;

}
