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
    void compare(const BigDecimal &other)
    {
        if((fbc != other.fbc) || (data.size() != other.data.size()))
            throw std::runtime_error("This big decimal implementaion works only with objects with equal ibc and fbc");
    }

    static void send_segment_to(std::ostream &os, uint32_t seg_value)
    {
        std::string segment = std::to_string(seg_value);
        os << std::string(base_length - segment.length(), '0'); // leading zeros
        os << segment;
    }

public:
    // create BigDecimal[0...0.0....0]
    BigDecimal(size_t ibc, size_t fbc, size_t precision) : fbc(fbc), precision(precision)
    {
        if(ibc + fbc == 0)
            throw std::runtime_error("Attempt to create BigDecimal with zero size");
        if(precision > base_length * fbc)
            throw std::runtime_error("BigDecimal::precision value don't fit in float blocks");

        size_t size = ibc + fbc;
        data.reserve(size);
        for(size_t i = 0; i < size; i++)
            data.push_back(0);
    }

    static size_t getBlockLength() { return base_length; }

    // create BigDecimal[0...01.0....0]
    static BigDecimal getOne(size_t ibc, size_t fbc, size_t precision)
    {
        BigDecimal value(ibc, fbc, precision);
        value.data[fbc] = 1;
        return value;
    }

    BigDecimal &operator+=(const BigDecimal &other)
    {
        compare(other);

        uint32_t sum = 0;
        for(size_t i = 0; i < data.size(); i++)
        {
            sum += data[i] + other.data[i];
            data[i] = sum % base;
            sum = (sum > base) ? 1 : 0;
        }

        return *this;
    }

    BigDecimal &operator/=(uint32_t divisor)
    {
        uint64_t carry = 0;
        for (size_t i = data.size(); i > 0;)
        {
            i--;
            uint64_t value = data[i] + carry * base;
            data[i] = static_cast<uint32_t>(value / divisor);
            carry = value % divisor;
        }

        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const BigDecimal &value)
    {
        size_t integer_part = value.data.size() - value.fbc;

        size_t i = 1;
        while((i < integer_part) & (value.data[value.data.size() - i] == 0))
            i++;
        os << value.data[value.data.size() - i]; // no leading zeros
        for(; i < integer_part; i++)
            send_segment_to(os, value.data[value.data.size() - i]);

        if(value.precision == 0)
            return os;
        
        os << '.';
        size_t fractional_part_size = (value.precision + BigDecimal::base_length - 1) / BigDecimal::base_length;
        for(int i = 1; i < fractional_part_size; i++)
            send_segment_to(os, value.data[value.fbc - i]);
        std::string last_segment = std::to_string(value.data[value.fbc - fractional_part_size]);
        last_segment = std::string(BigDecimal::base_length - last_segment.length(), '0') + last_segment;
        size_t last_segment_length = value.precision % BigDecimal::base_length;
        if(last_segment_length == 0)
            last_segment_length = BigDecimal::base_length;
        os << last_segment.substr(0, last_segment_length);

        return os;
    }
};

#endif // BIG_DECIMAL