#include <big_decimal.hpp>
#include <chrono>
#include <cmath>

uint32_t getInverseNlogN(uint32_t x)
{
    double low = 1;
    double high = x;

    while (high - low > 0.5)
    {
        double mid = (low + high) / 2;
        double value = mid * std::log10(mid);

        if (value < x)
            low = mid;
        else
            high = mid;
    }

    return static_cast<uint32_t>((low + high) / 2);
}

int main(int argc, char** argv)
{
    uint32_t precision = static_cast<uint32_t>(std::strtoul(argv[1], nullptr, 10));
    size_t fbc = (precision + BigDecimal::getBlockLength() - 1) / BigDecimal::getBlockLength();
    fbc += 2; // 2 additional blocks to account for the contribution of division remainders
    size_t N = 2 * getInverseNlogN(precision + 1); // (N/2)lg(N/2) = lg(2) + precision

    BigDecimal e = BigDecimal::getOne(1, fbc, precision);
    BigDecimal item = BigDecimal::getOne(1, fbc, precision);

    auto start = std::chrono::system_clock::now();
    for(uint32_t n = 1; n < N; n++)
    {
        item /= n;
        e += item;
    }
    auto end = std::chrono::system_clock::now();

    std::cout << e << std::endl;
    std::chrono::duration<double> time = end - start;
    std::cout << "time: " << time.count() << " seconds" << std::endl;
}
