#include "matrix.hpp"


// 64 -> 2048
int main()
{
    int size = 1024;
    Matrix<> A = Matrix<>::new_identity_matrix(size);
    A[3][4] = 7;
    Matrix<> B = Matrix<>::new_identity_matrix(size);
    B[2][3] = 5;
    Matrix<> reference(size);
    Matrix<> C(size);

    Matrix<> Q(10000);
    double start = omp_get_wtime();
    Q.transpose();
    std::cout << "transpose time: " << (omp_get_wtime() - start) << " s." << std::endl;
    start = omp_get_wtime();
    Q.transpose_block(10);
    std::cout << "block transpose time: " << (omp_get_wtime() - start) << " s." << std::endl;

    start = omp_get_wtime();
    Matrix<>::multiply_n3(reference, A, B);
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    start = omp_get_wtime();
    B.transpose_block(16);
    double middle = omp_get_wtime();
    Matrix<>::multiply_n3_transpose(C, A, B);
    std::cout << "tr: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    std::cout << (middle - start) << " s." << std::endl;
    B.transpose_block(16);

    start = omp_get_wtime();
    Matrix<>::multiply_n3_block(C, A, B, 16);
    std::cout << "block: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;
    
    // start = omp_get_wtime();
    // B.transpose_block(16);
    // middle = omp_get_wtime();
    // Matrix<>::multiply_n3_bt(C, A, B, 16);
    // std::cout << "bt: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    // std::cout << (middle - start) << " s." << std::endl;
    // B.transpose_block(16);

    start = omp_get_wtime();
    C = Matrix<>::multiply_Strassen(A, B, 64);
    std::cout << "St: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    Matrix<>::set_omp_threads(4);

    start = omp_get_wtime();
    Matrix<>::multiply_n3(C, A, B);
    std::cout << "omp n3: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    start = omp_get_wtime();
    B.transpose_block(16);
    middle = omp_get_wtime();
    Matrix<>::multiply_n3_transpose(C, A, B);
    std::cout << "omp tr: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    std::cout << (middle - start) << " s." << std::endl;
    B.transpose_block(16);

    start = omp_get_wtime();
    Matrix<>::multiply_n3_block(C, A, B, 16);
    std::cout << "omp block: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    start = omp_get_wtime();
    C = Matrix<>::multiply_Strassen(A, B, 64);
    std::cout << "omp St: " << ((C == reference) ? "good" : "fail") << std::endl;
    std::cout << (omp_get_wtime() - start) << " s." << std::endl;
}
