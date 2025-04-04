#include <big_decimal.hpp>

void BigDecimal::compare(const BigDecimal &other) const
{
    if((fbc != other.fbc) || (data.size() != other.data.size()))
        throw std::runtime_error("This big decimal implementaion works only with objects with same ibc and fbc");
}

BigDecimal::BigDecimal(size_t ibc, size_t fbc, size_t precision) : fbc(fbc), precision(precision)
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

size_t BigDecimal::getBlockLength() { return base_length; }

BigDecimal BigDecimal::getOne(size_t ibc, size_t fbc, size_t precision)
{
    BigDecimal value(ibc, fbc, precision);
    value.data[fbc] = 1;
    return value;
}

const std::vector<uint32_t> &BigDecimal::getData() const { return data; }

std::vector<uint32_t> &BigDecimal::getData() { return data; }

BigDecimal &BigDecimal::operator+=(const BigDecimal &other)
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
BigDecimal &BigDecimal::operator/=(uint32_t divisor)
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

BigDecimal operator*(const BigDecimal &left, const BigDecimal &right)
{
    left.compare(right);

    size_t size = left.data.size();
    size_t fbc = left.fbc;
    size_t ibc = size - fbc;
    BigDecimal product(ibc, fbc, left.precision);

    uint32_t *tail = new uint32_t[fbc];
    if(tail == nullptr)
        throw std::runtime_error("Can't allocate memory for BigDecimal multiplication");

    for (size_t i = 0; i < size; i++)
    {
        uint64_t carry = 0;
        for (size_t j = 0; j < size; j++)
        {
            uint32_t *product_data = nullptr;
            if(i + j < fbc)
            {
                // k = i + j - fbc < 0
                product_data = tail + fbc - i - j - 1;
            }
            else
            {
                size_t k = i + j - fbc;
                if(k == size)
                    break;
                product_data = &product.data[k];
            }

            uint64_t value = static_cast<uint64_t>(left.data[i]) * right.data[j] + *product_data + carry;
            *product_data = static_cast<uint32_t>(value % BigDecimal::base);
            carry = value / BigDecimal::base;
        }

        if(i < fbc)
            product.data[ibc + i] = static_cast<uint32_t>(carry); // j = size
    }

    delete[] tail;
    return product;
}

static void send_segment_to(std::ostream &os, uint32_t seg_value)
{
    std::string segment = std::to_string(seg_value);
    os << std::string(BigDecimal::getBlockLength() - segment.length(), '0'); // leading zeros
    os << segment;
}

std::ostream &operator<<(std::ostream &os, const BigDecimal &value)
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
