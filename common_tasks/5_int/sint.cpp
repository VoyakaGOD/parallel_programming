#include "config.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <stack>

double integrateV1(double A, double B)
{
    double h = B - A;
    int n = 1;
    double s0 = (f(A) + f(B)) / 2;
    double I_2n = s0 * h;
    double I_n = 0;

    do
    {
        I_n = I_2n;
        n *= 2;
        double s = s0;
        double hn = h / n;
        for(int i = 1; i < n; i++)
            s += f(A + hn * i);
        I_2n = s * hn;
    } 
    while (std::abs(I_2n - I_n) > epsilon * std::abs(I_2n));

    return I_2n;
}

double integrateV2(double A, double B, double fA, double fB)
{
    double C = (A + B) / 2;
    double fC = f(C);
    double h = C - A;
    double h2 = h / 2;

    double sAB = (fA + fB) * h;
    double sAC = (fA + fC) * h2;
    double sCB = (fC + fB) * h2;
    double sACB = sAC + sCB;

    if((A == C) || (C == B))
        return sACB;
    if(std::abs(sACB - sAB) > epsilon * std::abs(sACB))
        return integrateV2(A, C, fA, fC) + integrateV2(C, B, fC, fB);
    return sACB;
}

int stack_ptr = 0;
struct IntegrationData
{
    double A;
    double B;
    double fA;
    double fB;
    double sAB;
};

#define STACK_SIZE 1000
IntegrationData stack[STACK_SIZE];

#define IS_EMPTY (stack_ptr == 0)

#define PUSH(nA, nB, nfA, nfB, nsAB) \
    stack[stack_ptr].A = nA; \
    stack[stack_ptr].B = nB; \
    stack[stack_ptr].fA = nfA; \
    stack[stack_ptr].fB = nfB; \
    stack[stack_ptr].sAB = nsAB; \
    stack_ptr++;

#define POP(nA, nB, nfA, nfB, nsAB) \
    stack_ptr--; \
    nA = stack[stack_ptr].A; \
    nB = stack[stack_ptr].B; \
    nfA = stack[stack_ptr].fA; \
    nfB = stack[stack_ptr].fB; \
    nsAB = stack[stack_ptr].sAB


double integrateV3(double A, double B)
{
    double I = 0;
    double fA = f(A);
    double fB = f(B);
    double sAB = (fA + fB) * (B - A) / 2;

    while(true)
    {
        double C = (A + B) / 2;
        double fC = f(C);
        double h = (C - A) / 2;

        double sAC = (fA + fC) * h;
        double sCB = (fC + fB) * h;
        double sACB = sAC + sCB;

        if((std::abs(sACB - sAB) > epsilon * std::abs(sACB)) && (A != C) && (C != B))
        {
            PUSH(A, C, fA, fC, sAC);
            A = C;
            fA = fC;
            sAB = sCB;
        }
        else
        {
            I += sACB;
            if(IS_EMPTY)
                break;
            POP(A, B, fA, fB, sAB);
        }
    }

    return I;
}


double integrateV3Stack(double A, double B)
{
    std::stack<IntegrationData> stack;

    double I = 0;
    double fA = f(A);
    double fB = f(B);
    double sAB = (fA + fB) * (B - A) / 2;

    while(true)
    {
        double C = (A + B) / 2;
        double fC = f(C);
        double h = (C - A) / 2;

        double sAC = (fA + fC) * h;
        double sCB = (fC + fB) * h;
        double sACB = sAC + sCB;

        if((std::abs(sACB - sAB) > epsilon * std::abs(sACB)) && (A != C) && (C != B))
        {
            stack.push({A, C, fA, fC, sAC});
            A = C;
            fA = fC;
            sAB = sCB;
        }
        else
        {
            I += sACB;
            if(stack.empty())
                break;
            A = stack.top().A;
            B = stack.top().B;
            fA = stack.top().fA;
            fB = stack.top().fB;
            sAB = stack.top().sAB;
            stack.pop();
        }
    }

    return I;
}

int main(int argc, char** argv)
{
    std::cout << std::setprecision(10);

    auto cp1 = std::chrono::high_resolution_clock::now();
    double I1 = 0;
    if((argc > 1) && (argv[1] == std::string("V1")))
        I1 = integrateV1(from, to);
    else
        std::cout << "add flag <V1> to measure V1 algorithm" << std::endl;
    auto cp2 = std::chrono::high_resolution_clock::now();
    double I2 = integrateV2(from, to, f(from), f(to));
    auto cp3 = std::chrono::high_resolution_clock::now();
    double I3 = integrateV3(from, to);
    auto cp4 = std::chrono::high_resolution_clock::now();
    double I3s = integrateV3Stack(from, to);
    auto cp5 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = cp2 - cp1;
    std::cout << "V1:  " << I1 << ", time: " << elapsed.count() << " s" << std::endl;
    elapsed = cp3 - cp2;
    std::cout << "V2:  " << I2 << ", time: " << elapsed.count() << " s" << std::endl;
    elapsed = cp4 - cp3;
    std::cout << "V3:  " << I3 << ", time: " << elapsed.count() << " s" << std::endl;
    elapsed = cp5 - cp4;
    std::cout << "V3s: " << I3s << ", time: " << elapsed.count() << " s" << std::endl;
}
