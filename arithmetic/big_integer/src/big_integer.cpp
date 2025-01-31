#include "../include/big_integer.h"

big_integer &big_integer::trivial_multiplication::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier) const
{
    return first_multiplier *= second_multiplier;
}

big_integer &big_integer::Karatsuba_multiplication::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier) const
{
    if (first_multiplier.is_zero() || second_multiplier.is_one())
    {
        return first_multiplier;
    }
    if (second_multiplier.is_zero() || first_multiplier.is_one())
    {
        return first_multiplier = second_multiplier;
    }
    
    auto get_multiplier_half = [](big_integer const &multiplier, size_t size, size_t offset = 0)
        {
            std::vector<int> result(size, 0);
            for (size_t i = 0; i < size; ++i)
            {
                *reinterpret_cast<unsigned int *>(&result[i]) = multiplier.get_digit(i + offset);
            }
            
            while (!result.back() && !result.empty())
            {
                result.pop_back();
            }

            if (result.empty())
            {
                result = std::move(std::vector<int> { 0 });
            }

            return result;
        };
    
    auto digits_count = std::max(first_multiplier.get_digits_count(), second_multiplier.get_digits_count());
    if (digits_count <= 4)
    {
        return first_multiplier *= second_multiplier;
    }

    auto half_digits_count = digits_count / 2;
    
    auto first_multiplier_right = big_integer(get_multiplier_half(first_multiplier, half_digits_count), first_multiplier._allocator);
    auto first_multiplier_left = big_integer(get_multiplier_half(first_multiplier, half_digits_count, half_digits_count), first_multiplier._allocator);

    auto second_multiplier_right = big_integer(get_multiplier_half(second_multiplier, half_digits_count), first_multiplier._allocator);
    auto second_multiplier_left = big_integer(get_multiplier_half(second_multiplier, half_digits_count, half_digits_count), first_multiplier._allocator);

    big_integer first_multiplier_sum = first_multiplier_left + first_multiplier_right;
    big_integer second_multiplier_sum = second_multiplier_left + second_multiplier_right;

    auto coefficient_0 = multiply(first_multiplier_left, second_multiplier_left);
    auto coefficient_1 = multiply(first_multiplier_right, second_multiplier_right);
    auto coefficient_2 = multiply(first_multiplier_sum, second_multiplier_sum);

    return first_multiplier = (coefficient_0 << ((sizeof(int) << 3) * digits_count)) + ((coefficient_2 - coefficient_1 - coefficient_0) << ((sizeof(int) << 3) * (half_digits_count))) + coefficient_1;
}

big_integer &big_integer::Schonhage_Strassen_multiplication::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier) const
{
    throw not_implemented("big_integer &big_integer::Schonhage_Strassen_multiplication::multiply(big_integer &, big_integer const &)", "your code should be here...");
}

big_integer &big_integer::trivial_division::divide(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    switch (multiplication_rule)
    {
    
    default:
        return dividend /= divisor;
    
    }
}

big_integer &big_integer::trivial_division::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    switch (multiplication_rule)
    {
        default:
            return dividend %= divisor;
    }
}

big_integer &big_integer::Newton_division::divide(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Newton_division::divide(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Newton_division::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Newton_division::modulo(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Burnikel_Ziegler_division::divide(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Burnikel_Ziegler_division::divide(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Burnikel_Ziegler_division::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Burnikel_Ziegler_division::modulo(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

void big_integer::clear() noexcept
{
    _oldest_digit = 0;
    deallocate_with_guard(_other_digits);
    _other_digits = nullptr;
}

void big_integer::copy_from(big_integer const &other)
{
    _allocator = other._allocator;
    _oldest_digit = other._oldest_digit;
    if (other._other_digits == nullptr)
    {
        _other_digits = nullptr;
        return;
    }

    _other_digits = reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), *other._other_digits));
    std::memcpy(_other_digits, other._other_digits, sizeof(unsigned int) * (*other._other_digits));
}

void big_integer::move_from(big_integer &&other) noexcept
{
    _oldest_digit = std::exchange(other._oldest_digit, 0);
    _other_digits = std::exchange(other._other_digits, nullptr);
    _allocator = std::exchange(other._allocator, nullptr);
}

void big_integer::initialize_from(int const *digits, size_t digits_count, allocator *allocator)
{
    if (digits == nullptr)
    {
        throw std::logic_error("pointer to digits array must not be nullptr");
    }

    if (digits_count == 0)
    {
        throw std::logic_error("digits array length must be GT 0");
    }
    
    _oldest_digit = digits[digits_count - 1];
    _other_digits = digits_count == 1
        ? nullptr
        : reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), digits_count));
    
    if (_other_digits == nullptr)
    {
        return;
    }

    *_other_digits = static_cast<unsigned int>(digits_count);
    std::memcpy(_other_digits + 1, digits, sizeof(unsigned int) * (digits_count - 1));
}

void big_integer::initialize_from(std::vector<int> const &digits, size_t digits_count, allocator *allocator)
{
    if (digits.empty() || digits_count == 0)
    {
        throw std::logic_error("std::vector<int> of digits should not be empty");
    }

    _oldest_digit = digits[digits_count - 1];

    if (digits_count == 1)
    {
        _other_digits = nullptr;
        return;
    }

    _other_digits = reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), digits_count));
    *_other_digits = digits_count;

    for (auto i = 0; i < digits_count - 1; ++i)
    {
        _other_digits[1 + i] = *reinterpret_cast<unsigned int const *>(&digits[i]);
    }
}

void big_integer::initialize_from(std::vector<unsigned int> const &digits, size_t digits_count, allocator *allocator)
{
    if (digits.empty() || digits_count == 0)
    {
        throw std::logic_error("std::vector<int> of digits should not be empty");
    }

    _oldest_digit = digits[digits_count - 1];

    if (digits_count == 1)
    {
        _other_digits = nullptr;
        return;
    }

    _other_digits = reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), digits_count));
    *_other_digits = digits_count;

    for (auto i = 0; i < digits_count - 1; ++i)
    {
        _other_digits[1 + i] = digits[i];
    }
}

void big_integer::initialize_from(std::string const &value, size_t base, allocator *allocator)
{
    _oldest_digit = 0;
    _other_digits = nullptr;
    
    bool negative_flag = false;
    size_t pos = 0;
    
    if (value[pos] == '-')
    {
        negative_flag = true;
        ++pos;
    }

    while (value[pos] == '0')
    {
        ++pos;
    }

    auto char_to_int = [](char ch)
        {
            if ('0' <= ch && ch <= '9')
            {
                return ch - '0';
            }
            if ('A' <= ch && ch <= 'Z')
            {
                return ch - 'A' + 10;
            }
            if ('a' <= ch && ch <= 'z')
            {
                return ch - 'a' + 36;
            }
            
            throw std::logic_error("Invalid digit");
        };
    
    for (; pos < value.size(); ++pos)
    {
        int digit = char_to_int(value[pos]);
        
        if (digit >= base)
        {
            throw std::logic_error("Invalid digit");
        }
        
        *this *= big_integer(std::vector<int> { *reinterpret_cast<int *>(&base) }, _allocator);
        *this += big_integer(std::vector<int> { digit }, _allocator);
    }
    
    if (negative_flag)
    {
        change_sign();
    }
}

std::pair<big_integer, big_integer> big_integer::common_division(big_integer const &dividend, big_integer const &divisor)
{
    if (divisor.is_zero())
    {
        throw std::logic_error("Can't divide by zero");
    }
    if (dividend.is_zero())
    {
        return std::make_pair(dividend, dividend);
    }

    if (divisor.is_one())
    {
        return std::make_pair(dividend, big_integer("0"));
    }
    
    if (dividend.sign() == -1)
    {
        if (divisor.sign() == -1)
        {
            return common_division(-dividend, -divisor);;
        }

        auto result = common_division(-dividend, divisor);
        return std::make_pair(-result.first, -result.second);
    }
    else if (divisor.sign() == -1)
    {
        auto result = common_division(dividend, -divisor);
        return std::make_pair(result.first.change_sign(), result.second);
    }

    if (dividend < divisor)
    {
        return std::make_pair(big_integer(std::vector<int> { 0 }, dividend._allocator), dividend);
    }

    if (dividend == divisor)
    {
        return std::make_pair(big_integer(std::vector<int> { 1 }, dividend._allocator), big_integer(std::vector<int> { 0 }, dividend._allocator));
    }

    unsigned int constexpr shift = sizeof(int) << 3;

    auto const lhs_digits_count = dividend.get_digits_count();
    auto const rhs_digits_count = divisor.get_digits_count();

    std::vector<int> result_digits(1, 0);
    
    big_integer remainder(std::vector<int> { 0 }, dividend._allocator);

    for (int i = lhs_digits_count - 1; i >= 0; --i)
    {
        auto lhs_digit = dividend.get_digit(i);
        remainder <<= shift;
        remainder += (1 << ((sizeof(int) << 3) - 1)) & lhs_digit
            ? big_integer(std::vector<int> { *reinterpret_cast<int *>(&lhs_digit), 0 }, remainder._allocator)
            : big_integer(std::vector<int> { *reinterpret_cast<int *>(&lhs_digit) }, dividend._allocator);

        big_integer deducted(std::vector<int> { 0 }, dividend._allocator);

        if (remainder >= divisor)
        {
            unsigned int digit = 0;

            for (unsigned int j = 1 << (shift - 1); j > 0; j >>= 1)
            {
                big_integer mult_result = divisor * big_integer(std::vector<int> { *reinterpret_cast<int *>(&j), 0 }); 

                if (mult_result + deducted <= remainder)
                {
                    deducted += mult_result;
                    digit += j;
                }
            }

            remainder -= deducted;
            result_digits.push_back(*reinterpret_cast<int *>(&digit));
        }
        else
        {
            result_digits.push_back(0);
        }
    }

    std::reverse(result_digits.begin(), result_digits.end());
    while (!result_digits.back() && result_digits.size())
    {
        result_digits.pop_back();
    }

    if (!result_digits.size())
    {
        result_digits = std::move(std::vector<int> { 0 });
    }

    if (result_digits.back() & (1 << ((sizeof(int) << 3) - 1)))
    {
        result_digits.push_back(0);
    }

    return std::make_pair(big_integer(result_digits, dividend._allocator), remainder);
}

big_integer &big_integer::change_sign() noexcept
{
    _oldest_digit ^= (1 << ((sizeof(int) << 3) - 1));

    return *this;
}

inline int big_integer::get_digits_count() const noexcept
{
    return static_cast<int>(_other_digits == nullptr
        ? 1
        : *_other_digits);
}

inline int big_integer::sign() const noexcept
{
    if (is_zero())
    {
        return 0;
    }

    return 1 - (static_cast<int>((*reinterpret_cast<unsigned int const *>(&_oldest_digit) >> ((sizeof(int) << 3) - 1))) << 1);
}

inline bool big_integer::is_zero() const noexcept
{
    return _oldest_digit == 0 && _other_digits == nullptr;
}

inline bool big_integer::is_one() const noexcept
{
    return _oldest_digit == 1 && _other_digits == nullptr;
}

inline unsigned int big_integer::get_digit(
    size_t position) const noexcept
{
    if (_other_digits == nullptr)
    {
        return position == 0
            ? _oldest_digit
            : 0;
    }

    int const digits_count = get_digits_count();
    if (position < digits_count - 1)
    {
        return _other_digits[position + 1];
    }

    if (position == digits_count - 1)
    {
        return _oldest_digit;
    }

    return 0;
}

big_integer::big_integer(int const *digits, size_t digits_count, allocator *allocator):
    _allocator(allocator), _oldest_digit(0), _other_digits(nullptr)
{
    initialize_from(digits, digits_count, allocator);
}

big_integer::big_integer(std::vector<int> const &digits, allocator *allocator):
    _allocator(allocator), _oldest_digit(0), _other_digits(nullptr)
{
    initialize_from(digits, digits.size(), allocator);
}

big_integer::big_integer(std::string const &value_as_string, size_t base, allocator *allocator):
    _allocator(allocator), _oldest_digit(0), _other_digits(nullptr)
{
    initialize_from(value_as_string, base, allocator);
}

big_integer::~big_integer()
{
    clear();
}

big_integer::big_integer(big_integer const &other)
{
    copy_from(other);
}

big_integer &big_integer::operator=(big_integer const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other);
    }
    
    return *this;
}

big_integer::big_integer(big_integer &&other) noexcept
{
    move_from(std::move(other));
}

big_integer &big_integer::operator=(big_integer &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }

    return *this;
}

bool big_integer::operator==(big_integer const &other) const
{
    if (get_digits_count() != other.get_digits_count())
    {
        return false;
    }

    auto const digits_count = std::max(get_digits_count(), other.get_digits_count());

    for (size_t i = 0; i < digits_count; ++i)
    {
        if (get_digit(i) != other.get_digit(i))
        {
            return false;
        }
    }

    return true;
}

bool big_integer::operator!=(
    big_integer const &other) const
{
    return !(*this == other);
}

bool big_integer::operator<(
    big_integer const &other) const
{
    return !(*this > other || *this == other);
}

bool big_integer::operator>(big_integer const &other) const
{
    if (sign() != other.sign())
    {
        return sign() > other.sign();
    }

    bool is_positive = sign() == 1;
    
    if (get_digits_count() != other.get_digits_count())
    {
        return is_positive ? get_digits_count() > other.get_digits_count() : get_digits_count() < other.get_digits_count();
    }

    if (_oldest_digit != other._oldest_digit)
    {
        return is_positive ? _oldest_digit > other._oldest_digit : _oldest_digit < other._oldest_digit;
    }

    auto const digits_count = std::max(get_digits_count(), other.get_digits_count());

    for (int i = digits_count - 1; i >= 0; --i)
    {
        auto const lhs_digit = get_digit(i);
        auto const rhs_digit = other.get_digit(i);

        if (lhs_digit != rhs_digit)
        {
            return is_positive ? lhs_digit > rhs_digit : lhs_digit < rhs_digit;
        }
    }
    
    return false;
}

bool big_integer::operator<=(big_integer const &other) const
{
    return !(*this > other);
}

bool big_integer::operator>=(big_integer const &other) const
{
    return *this > other || *this == other;
}

big_integer big_integer::operator-() const
{
    return big_integer(*this).change_sign();
}

big_integer &big_integer::operator+=(big_integer const &other)
{
    if (other.is_zero())
    {
        return *this;
    }

    if (is_zero())
    {
        return *this = other;
    }

    if (sign() == -1)
    {
        change_sign();
        *this += -other;
        return change_sign();
    }

    if (other.sign() == -1)
    {
        return *this -= -other;
    }

    auto const first_digits_count = get_digits_count();
    auto const second_digits_count = other.get_digits_count();
    auto const digits_count = std::max(get_digits_count(), other.get_digits_count());

    unsigned int operation_result = 0;

    int constexpr shift = sizeof(unsigned int) << 2;
    int constexpr mask = (1 << shift) - 1;

    std::vector<int> result_digits(digits_count + 1, 0);

    for (size_t i = 0; i < digits_count; ++i)
    {
        unsigned int first_value_digit = get_digit(i);
        unsigned int second_value_digit = other.get_digit(i);

        for (int j = 0; j < 2; ++j)
        {
            operation_result += (first_value_digit & mask) + (second_value_digit & mask);
            first_value_digit >>= shift;
            second_value_digit >>= shift;

            *reinterpret_cast<unsigned int *>(&result_digits[i]) |= ((operation_result & mask) << shift * j);
            operation_result >>= shift;
        }
    }

    result_digits.back() += *reinterpret_cast<int *>(&operation_result);

    auto maybe_overflowed_digit_ptr = reinterpret_cast<unsigned int *>(&*(result_digits.end() - 2));
    if (*maybe_overflowed_digit_ptr >> ((sizeof(unsigned int) << 3) - 1))
    {
        *maybe_overflowed_digit_ptr ^= (1 << ((sizeof(unsigned int) << 3) - 1));
        ++result_digits.back();
    }
    
    auto result_digits_count = result_digits.size();
    if (result_digits.back() == 0)
    {
        --result_digits_count;
    }
    
    clear();
    initialize_from(result_digits, result_digits_count, _allocator);

    return *this;
}

big_integer big_integer::operator+(big_integer const &other) const
{
    return big_integer(*this) += other;
}

big_integer big_integer::operator+(std::pair<big_integer, allocator *> const &other) const
{
    big_integer copy = other.first;
    copy._allocator = other.second;

    return copy + *this;
}

big_integer &big_integer::operator-=(big_integer const &other)
{
    if (other.is_zero())
    {
        return *this;
    }
    if (is_zero())
    {
        return *this = -other;
    }
    
    if (sign() == 1 && other.sign() == -1)
    {
        return *this += -other;
    }

    if (sign() == -1)
    {
        if (other.sign() == 1)
        {
            return *this = (-*this + other).change_sign();
        }

        return *this = (-other) - (-*this);
    }

    if (other > *this)
    {
        return *this = -(other - *this);
    }

    auto const digits_count = std::max(get_digits_count(), other.get_digits_count());

    unsigned int borrow = 0;

    std::vector<int> result_digits(digits_count, 0);
    for(int i = 0; i < digits_count; ++i)
    {
        unsigned int first_value_digit = get_digit(i);
        unsigned int second_value_digit = other.get_digit(i);

        if (second_value_digit == UINT32_MAX)
        {
            if (first_value_digit == UINT32_MAX)
            {
                *reinterpret_cast<unsigned int *>(&result_digits[i]) = borrow ? UINT32_MAX : 0;
            }
            else
            {
                *reinterpret_cast<unsigned int *>(&result_digits[i]) = first_value_digit - borrow + 1;
                borrow = 1;
            }
        }
        else if (first_value_digit < second_value_digit + borrow)
        {
            *reinterpret_cast<unsigned int *>(&result_digits[i]) = first_value_digit + (UINT32_MAX - second_value_digit - borrow) + 1;
            borrow = 1;
        }
        else
        {
            *reinterpret_cast<unsigned int *>(&result_digits[i]) = first_value_digit - second_value_digit - borrow;
            borrow = 0;
        }
    }

    while (!result_digits.back() && result_digits.size())
    {
        result_digits.pop_back();
    }

    if (!result_digits.size())
    {
        result_digits = std::move(std::vector<int> { 0 });
    }

    if (result_digits.back() & (1 << ((sizeof(int) << 3) - 1)))
    {
        result_digits.push_back(0);
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator-(big_integer const &other) const
{
    return big_integer(*this) -= other;
}

big_integer big_integer::operator-(std::pair<big_integer, allocator *> const &other) const
{
    big_integer copy = other.first;
    copy._allocator = other.second;

    return (copy - *this).change_sign();
}

big_integer &big_integer::operator*=(big_integer const &other)
{
    if (other.is_zero())
    {
        return *this = other;
    }

    if (is_zero())
    {
        return *this;
    }

    if (other.is_one())
    {
        return *this;
    }

    if (is_one())
    {
        return *this = other;
    }

    if (sign() == -1 && other.sign() == -1)
    {
        change_sign();
        return *this *= -other;
    }
    else if (sign() == -1)
    {
        change_sign();
        *this *= other;
        return change_sign();
    }
    else if (other.sign() == -1)
    {
        *this *= -other;
        return change_sign();
    }

    unsigned int operation_result = 0;

    auto const first_digits_count = get_digits_count();
    auto const second_digits_count = other.get_digits_count();
    auto const max_digits_count = first_digits_count + second_digits_count;

    int constexpr shift = sizeof(unsigned int) << 2;
    int constexpr mask = (1 << shift) - 1;

    std::vector<int> result_digits(max_digits_count, 0);
    std::vector<unsigned int> half_digits_result(2 * max_digits_count, 0);

    for (int i = 0; i < 2 * first_digits_count; ++i)
    {
        unsigned int lhs_digit = get_digit(i / 2);
        unsigned int lhs_digit_half = i & 1 
            ? lhs_digit >> shift 
            : lhs_digit & mask;

        for (int j = 0; j < 2 * second_digits_count; ++j)
        {
            unsigned int rhs_digit = other.get_digit(j / 2);
            unsigned int rhs_digit_half = j & 1 
                ? rhs_digit >> shift 
                : rhs_digit & mask;

            operation_result += lhs_digit_half * rhs_digit_half;
            half_digits_result[i + j] += operation_result;
            operation_result = half_digits_result[i + j] >> shift;
            half_digits_result[i + j] &= mask;
        }

        half_digits_result[i + 2 * second_digits_count] = operation_result;
        operation_result = 0;
    }

    for (int i = 0; i < max_digits_count; ++i)
    {
        result_digits[i] = (half_digits_result[2 * i + 1] << shift) + half_digits_result[2 * i];
    }

    while (!result_digits.back() && result_digits.size())
    {
        result_digits.pop_back();
    }

    if (!result_digits.size())
    {
        result_digits = std::move(std::vector<int> { 0 });
    }

    if (result_digits.back() & (1 << ((sizeof(int) << 3) - 1)))
    {
        result_digits.push_back(0);
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator*(big_integer const &other) const
{
    return big_integer(*this) *= other;
}

big_integer big_integer::operator*(std::pair<big_integer, allocator *> const &other) const
{
    big_integer copy = other.first;
    copy._allocator = other.second;

    return copy * *this;
}

big_integer &big_integer::operator/=(big_integer const &other)
{
    return *this = common_division(*this, other).first;
}

big_integer big_integer::operator/(big_integer const &other) const
{
    return big_integer(*this) /= other;
}

big_integer big_integer::operator/(std::pair<big_integer, allocator *> const &other) const
{
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &other)
{
    return *this = common_division(*this, other).second;
}

big_integer big_integer::operator%(big_integer const &other) const
{
    return big_integer(*this) %= other;
}

big_integer big_integer::operator%(std::pair<big_integer, allocator *> const &other) const
{
    return *this;
}

big_integer big_integer::operator~() const
{
    auto const digits_count = get_digits_count();
    std::vector<int> result_digits(digits_count, 0);
    
    for (size_t i = 0; i < digits_count; ++i)
    {
        result_digits[i] = ~get_digit(i);    
    }

    return big_integer(result_digits, _allocator);
}

big_integer &big_integer::operator&=(big_integer const &other)
{
    auto const digits_count = std::min(get_digits_count(), other.get_digits_count());
    std::vector<int> result_digits(digits_count, 0);

    for (size_t i = 0; i < digits_count; ++i)
    {
        result_digits[i] = get_digit(i) & other.get_digit(i); 
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator&(big_integer const &other) const
{
    return big_integer(*this) &= other;
}

big_integer big_integer::operator&(std::pair<big_integer, allocator *> const &other) const
{
    big_integer copy = other.first;
    copy._allocator = other.second;

    return copy & *this;
}

big_integer &big_integer::operator|=(big_integer const &other)
{
    auto const digits_count = get_digits_count();
    std::vector<int> result_digits(digits_count, 0);

    for (size_t i = 0; i < digits_count; ++i)
    {
        result_digits[i] = get_digit(i) | other.get_digit(i); 
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator|(big_integer const &other) const
{
    return big_integer(*this) |= other;
}

big_integer big_integer::operator|(std::pair<big_integer, allocator *> const &other) const
{
    big_integer copy = other.first;
    copy._allocator = other.second;

    return copy | *this;
}

big_integer &big_integer::operator^=(big_integer const &other)
{
    auto const digits_count = std::max(get_digits_count(), other.get_digits_count());
    std::vector<int> result_digits(digits_count, 0);

    for (size_t i = 0; i < digits_count; ++i)
    {
        result_digits[i] = get_digit(i) ^ other.get_digit(i); 
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator^(big_integer const &other) const
{
    return big_integer(*this) ^= other;
}

big_integer big_integer::operator^(std::pair<big_integer, allocator *> const &other) const
{
    big_integer copy = other.first;
    copy._allocator = other.second;

    return copy ^ *this;
}

big_integer &big_integer::operator<<=(size_t shift)
{
    if (is_zero() || !shift)
    {
        return *this;
    }

    if (sign() == -1)
    {
        change_sign();
        *this <<= shift;
        return change_sign();
    }

    unsigned int constexpr power = sizeof(int) << 3;
    unsigned int big_shift = shift / power;
    shift %= power;

    auto const digits_count = get_digits_count();
    std::vector<int> result_digits(digits_count + big_shift, 0);

    unsigned int carry = 0;

    for (size_t i = 0; i < digits_count; ++i)
    {
        unsigned int digit = get_digit(i);
        *reinterpret_cast<unsigned int *>(&result_digits[i + big_shift]) = (digit << shift) | carry;
        carry = power - shift == power ? 0 : digit >> (power - shift);
    }

    if (carry)
    {
        result_digits.push_back(static_cast<int>(carry));
    }

    if (result_digits.back() & (1 << ((sizeof(int) << 3) - 1)))
    {
        result_digits.push_back(0);
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator<<(size_t shift) const
{
    return big_integer(*this) <<= shift;
}

big_integer big_integer::operator<<(std::pair<size_t, allocator *> const &shift) const
{
    big_integer result(*this);
    result._allocator = shift.second;

    return result <<= shift.first;
}

big_integer &big_integer::operator>>=(size_t shift)
{
    if (is_zero() || shift == 0)
    {
        return *this;
    }

    unsigned int constexpr power = sizeof(int) << 3;
    auto big_shift = shift / power;
    shift %= power;

    auto digits_count = get_digits_count();
    if (digits_count <= big_shift)
    {
        clear();
        initialize_from(std::vector<int>{ 0 }, 1, _allocator);

        return *this;
    }

    std::vector<int> result_digits(digits_count - big_shift);

    unsigned int remainder = 0;

    for (int i = static_cast<int>(big_shift); i < digits_count; ++i)
    {
        unsigned int value_digit = get_digit(i);

        *reinterpret_cast<unsigned int *>(&result_digits[i - big_shift]) = (value_digit >> shift) | remainder;
        remainder = power - shift == power ? 0 : value_digit << (power - shift);
    }

    while (result_digits.back() == 0 && result_digits.size())
    {
        result_digits.pop_back();
    }

    if (!result_digits.size())
    {
        result_digits = std::move(std::vector<int> { 0 });
    }

    clear();
    initialize_from(result_digits, result_digits.size(), _allocator);

    return *this;
}

big_integer big_integer::operator>>(size_t shift) const
{
    return big_integer(*this) >>= shift;
}

big_integer big_integer::operator>>(std::pair<size_t, allocator *> const &other) const
{
    big_integer result(*this);
    result._allocator = other.second;

    return result >>= other.first;
}

big_integer &big_integer::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier,
    allocator *allocator,
    big_integer::multiplication_rule multiplication_rule)
{
    first_multiplier._allocator = allocator;
    
    switch (multiplication_rule)
    {
    
    case multiplication_rule::Karatsuba:
        return Karatsuba_multiplication().multiply(first_multiplier, second_multiplier);
    default:
        return trivial_multiplication().multiply(first_multiplier, second_multiplier);
    
    }
}

big_integer big_integer::multiply(
    big_integer const &first_multiplier,
    big_integer const &second_multiplier,
    allocator *allocator,
    big_integer::multiplication_rule multiplication_rule)
{
    big_integer copy = first_multiplier;
    copy._allocator = allocator;
    
    switch (multiplication_rule)
    {
    
    case multiplication_rule::Karatsuba:
        return Karatsuba_multiplication().multiply(copy, second_multiplier);
    default:
        return trivial_multiplication().multiply(copy, second_multiplier);
    
    }
}

big_integer &big_integer::divide(
    big_integer &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    dividend._allocator = allocator;

    switch (division_rule)
    {
    
    default:
        return trivial_division().divide(dividend, divisor, multiplication_rule);

    }
}

big_integer big_integer::divide(
    big_integer const &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    big_integer copy = dividend;
    copy._allocator = allocator;

    switch (division_rule)
    {
    
    default:
        return trivial_division().divide(copy, divisor, multiplication_rule);

    }
}

big_integer &big_integer::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    dividend._allocator = allocator;

    switch (division_rule)
    {
    
    default:
        return trivial_division().modulo(dividend, divisor, multiplication_rule);

    }
}

big_integer big_integer::modulo(
    big_integer const &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    big_integer copy = dividend;
    copy._allocator = allocator;

    switch (division_rule)
    {
    
    default:
        return trivial_division().modulo(copy, divisor, multiplication_rule);

    }
}

big_integer big_integer::abs(big_integer const &number) noexcept
{
    return number >= big_integer("0") ? number : -number;
}

big_integer big_integer::max(big_integer const &one, big_integer const &another) noexcept
{
    return one > another ? one : another;
}

big_integer big_integer::min(big_integer const &one, big_integer const &another) noexcept
{
    return one < another ? one : another;
}

big_integer big_integer::factorial(big_integer const &number)
{
    if (number < big_integer("0"))
    {
        throw std::invalid_argument("Attempt to get factorial from negative number");
    }
    
    if (number.is_zero() || number.is_one())
    {
        return big_integer("1");
    }

    return number * factorial(number - big_integer("1"));
}

std::ostream &operator<<(std::ostream &stream, big_integer const &value)
{
    std::string result;
    
    big_integer big_number(value);
    int sign = big_number.sign();
    
    if (sign == -1)
    {
        big_number.change_sign();
    }
    
    size_t constexpr big_modulus_zeros_cnt = 9;
    int big_modulus = 1;
    
    for (size_t i = 0; i < big_modulus_zeros_cnt; ++i)
    {
        big_modulus *= 10;
    }
    
    while (!big_number.is_zero())
    {
        auto pair = big_integer::common_division(big_number, big_integer(std::vector<int> { big_modulus } , value._allocator));
        
        unsigned int tmp = pair.second.get_digit(0);
        for (size_t i = 0; i < big_modulus_zeros_cnt; ++i)
        {
            if (!tmp && pair.first.is_zero())
            {
                break;
            }

            result.push_back(tmp % 10 + '0');
            tmp /= 10;
        }
        
        big_number = pair.first;
    }
    
    if (value.is_zero())
    {
        result.push_back('0');
    }
    
    if (sign == -1)
    {
        result.push_back('-');
    }
    
    std::reverse(result.begin(), result.end());
    
    stream << result;
    return stream;
}

std::istream &operator>>(std::istream &stream, big_integer &value)
{
    std::string string;
    stream >> string;

    value.clear();
    value.initialize_from(string, 10, value._allocator);

    return stream;
}

[[nodiscard]] allocator *big_integer::get_allocator() const noexcept
{
    return _allocator;
}