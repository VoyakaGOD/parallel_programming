// simple implementation of non-negative big decimal via std::vector<uint32_t> 
// this compact library created for special purposes
// ibc - integer blocks count
// fbc - fractional blocks count
// precision - number of digits when send to stream

#ifndef BIG_DECIMAL
#define BIG_DECIMAL

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdint>

class BigDecimal
{
private:
    static const uint32_t base = 100000000ul;
    static const size_t base_length = 8;

private:
    size_t fbc;
    size_t precision;
    std::vector<uint32_t> data;

private:
    void compare(const BigDecimal &other) const;

public:
    // create BigDecimal[0...0.0....0]
    BigDecimal(size_t ibc, size_t fbc, size_t precision);

    static size_t getBlockLength();

    // create BigDecimal[0...01.0....0]
    static BigDecimal getOne(size_t ibc, size_t fbc, size_t precision);

    BigDecimal &operator+=(const BigDecimal &other);

    BigDecimal &operator/=(uint32_t divisor);

    friend BigDecimal operator*(const BigDecimal &left, const BigDecimal &right);

    friend std::ostream &operator<<(std::ostream &os, const BigDecimal &value);
};

#endif // BIG_DECIMAL
