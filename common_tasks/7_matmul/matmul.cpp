#include "matrix32.hpp"

// 64 -> 2048
int main()
{
    // int size = 1024;
    // Matrix<> A = Matrix<>::new_identity_matrix(size);
    // A[3][4] = 7;
    // Matrix<> B = Matrix<>::new_identity_matrix(size);
    // B[2][3] = 5;
    // Matrix<> reference(size);
    // Matrix<> C(size);

    // Matrix<> Q(10000);
    // double start = omp_get_wtime();
    // Q.transpose();
    // std::cout << "transpose time: " << (omp_get_wtime() - start) << " s." << std::endl;
    // start = omp_get_wtime();
    // Q.transpose_block(10);
    // std::cout << "block transpose time: " << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // Matrix<>::multiply_n3(reference, A, B);
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // B.transpose_block(16);
    // double middle = omp_get_wtime();
    // Matrix<>::multiply_n3_transpose(C, A, B);
    // std::cout << "tr: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    // std::cout << (middle - start) << " s." << std::endl;
    // B.transpose_block(16);

    // start = omp_get_wtime();
    // Matrix<>::multiply_n3_block(C, A, B, 16);
    // std::cout << "block: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;
    
    // start = omp_get_wtime();
    // B.transpose_block(16);
    // middle = omp_get_wtime();
    // Matrix<>::multiply_n3_bt(C, A, B, 16);
    // std::cout << "bt: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    // std::cout << (middle - start) << " s." << std::endl;
    // B.transpose_block(16);

    // start = omp_get_wtime();
    // C = Matrix<>::multiply_Strassen(A, B, 64);
    // std::cout << "St: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // Matrix<>::multiply_n3_omp_simd(C, A, B);
    // std::cout << "n3 omp simd: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // Matrix32 A32 = A;
    // Matrix32 B32 = B;
    // Matrix32 C32(size);
    // start = omp_get_wtime();
    // Matrix32::multiply_n3_avx512(C32, A32, B32);
    // std::cout << "n3 avx512: " << ((C32 == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // B32.transpose_block(16);
    // middle = omp_get_wtime();
    // Matrix32::multiply_n3_avx512_tr(C32, A32, B32);
    // std::cout << "n3 avx512 tr: " << ((C32 == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    // std::cout << (middle - start) << " s." << std::endl;
    // B32.transpose_block(16);

    // start = omp_get_wtime();
    // C32 = Matrix32::multiply_Strassen_avx512(A32, B32, 64);
    // std::cout << "St avx512: " << ((C32 == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // Matrix<>::set_omp_threads(8);
    // std::cout << "\n*Threads = 8:" << std::endl;

    // start = omp_get_wtime();
    // Matrix<>::multiply_n3(C, A, B);
    // std::cout << "omp n3: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // B.transpose_block(16);
    // middle = omp_get_wtime();
    // Matrix<>::multiply_n3_transpose(C, A, B);
    // std::cout << "omp tr: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    // std::cout << (middle - start) << " s." << std::endl;
    // B.transpose_block(16);

    // start = omp_get_wtime();
    // Matrix<>::multiply_n3_block(C, A, B, 4);
    // std::cout << "omp block: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // C = Matrix<>::multiply_Strassen(A, B, 64);
    // std::cout << "omp St: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // Matrix<>::multiply_n3_omp_simd(C, A, B);
    // std::cout << "omp n3 omp simd: " << ((C == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // Matrix32::multiply_n3_avx512(C32, A32, B32);
    // std::cout << "omp n3 avx512: " << ((C32 == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // start = omp_get_wtime();
    // B32.transpose_block(16);
    // middle = omp_get_wtime();
    // Matrix32::multiply_n3_avx512_tr(C32, A32, B32);
    // std::cout << "omp n3 avx512 tr: " << ((C32 == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s, transpose: ";
    // std::cout << (middle - start) << " s." << std::endl;
    // B32.transpose_block(16);

    // start = omp_get_wtime();
    // C32 = Matrix32::multiply_Strassen_avx512(A32, B32, 64);
    // std::cout << "omp St avx512: " << ((C32 == reference) ? "good" : "fail") << std::endl;
    // std::cout << (omp_get_wtime() - start) << " s." << std::endl;

    // int size = 1024;
    // Matrix<> A = Matrix<>::new_identity_matrix(size);
    // A[3][4] = 7;
    // Matrix<> B = Matrix<>::new_identity_matrix(size);
    // B[2][3] = 5;
    // Matrix<> reference(size);
    // Matrix<> C(size);

    // Matrix<>::set_omp_threads(8);
    int bsize = 128;
    for(int size = 64; size <= 2048; size *= 2)
    {
        Matrix<> A = Matrix<>::new_identity_matrix(size);
        // A[3][4] = 7;
        Matrix<> B = Matrix<>::new_identity_matrix(size);
        // B[2][3] = 5;
        Matrix<> C(size);
        double start = omp_get_wtime();
        // B.transpose();
        C = Matrix<>::multiply_Strassen(A, B, bsize);
        std::cout << (omp_get_wtime() - start) << ", ";
        std::cout.flush();
    }
}
