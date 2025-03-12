#include <big_decimal.hpp>

int main(int argc, char** argv)
{
    uint32_t precision = static_cast<uint32_t>(std::strtoul(argv[1], nullptr, 10));
    size_t fbc = (precision + BigDecimal::getBlockLength() - 1) / BigDecimal::getBlockLength();
    fbc += 2; // 2 additional blocks to account for the contribution of division remainders

    BigDecimal e = BigDecimal::getOne(1, fbc, precision);
    BigDecimal item = BigDecimal::getOne(1, fbc, precision);
    for(uint32_t n = 1; n < precision; n++)
    {
        item /= n;
        e += item;
    }

    std::cout << e << std::endl;
}
