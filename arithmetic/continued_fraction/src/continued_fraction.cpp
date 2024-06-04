#include "../include/continued_fraction.h"

std::vector<big_integer> continued_fraction::to_continued_fraction_representation(fraction const &value)
{
    std::vector<big_integer> result;

    fraction copy = value;

    big_integer const one("1");
    while (true)
    {
        auto integer_part = copy.get_numerator() / copy.get_denominator();
        result.push_back(integer_part);

        copy -= fraction(integer_part, one);
        if (copy.get_denominator() == one)
        {
            break;
        }
        copy = fraction(copy.get_denominator(), copy.get_numerator());
    }

    return result;
}

fraction continued_fraction::from_continued_fraction_representation(std::vector<big_integer> const &continued_fraction_representation)
{
    size_t continued_fraction_size = continued_fraction_representation.size();
    if (!continued_fraction_size)
    {
        throw std::invalid_argument("Cannot make fraction from empty vector of continued fraction representation");
    }

    big_integer const one_integer("1");

    fraction result(continued_fraction_representation.back(), one_integer);
    fraction one_fraction(one_integer, one_integer);

    for (int i = continued_fraction_representation.size() - 2; i >= 0; --i)
    {
        result = one_fraction / result;
        result += fraction(continued_fraction_representation[i], one_integer);
    }

    return result;
}

std::vector<fraction> continued_fraction::to_convergents_series(fraction const &value)
{
    return to_convergents_series(to_continued_fraction_representation(value));
}

std::vector<fraction> continued_fraction::to_convergents_series(std::vector<big_integer> const &continued_fraction_representation)
{
    std::vector<fraction> result;

    size_t representation_size = continued_fraction_representation.size();
    if (representation_size == 0)
    {
        throw std::invalid_argument("Cannot make convergents series from empty vector of continued fraction representation");
    }

    big_integer const one_integer("1");

    for (size_t i = 0; i < representation_size; ++i)
    {
        if (i == 0)
        {
            result.push_back(fraction(continued_fraction_representation[0], one_integer));
            continue;
        }
        if (i == 1)
        {
            result.push_back(fraction(continued_fraction_representation[0] * continued_fraction_representation[1] + one_integer, continued_fraction_representation[1]));
            continue;
        }

        result.push_back(fraction(continued_fraction_representation[i] * result[i - 1].get_numerator() + result[i - 2].get_numerator(), continued_fraction_representation[i] * result[i - 1].get_denominator() + result[i - 2].get_denominator()));
    }

    return result;
}

std::vector<bool> continued_fraction::to_Stern_Brokot_tree_path(fraction const &value)
{
    std::vector<bool> result;
    
    big_integer first("0"), second("1"), third("1"), fourth("0");
    fraction current(third, second);
    while (value != current)
    {
        if (value < current)
        {
            result.push_back(false);
            third = current.get_numerator();
            fourth = current.get_denominator();
            current = fraction(first + third, second + fourth);
        }
        else
        {
            result.push_back(true);
            first = current.get_numerator();
            second = current.get_denominator();
            current = fraction(first + third, second + fourth);
        }
    }

    return result;
}

fraction continued_fraction::from_Stern_Brokot_tree_path(std::vector<bool> const &path)
{
    big_integer first("0"), second("1"), third("1"), fourth("0");
    fraction result(second, third);
    for (bool go_right: path)
    {
        if (go_right)
        {
            third = result.get_numerator();
            fourth = result.get_denominator();
            result = fraction(first + third, second + fourth);
        }
        else
        {
            first = result.get_numerator();
            second = result.get_denominator();
            result = fraction(first + third, second + fourth);
        }
    }

    return fraction(result.get_denominator(), result.get_numerator());
}

std::vector<bool> continued_fraction::to_Calkin_Wilf_tree_path(fraction const &value)
{
    std::vector<bool> result;
    
    fraction current = value;
    big_integer const one_integer("1");
    fraction const one_fraction(one_integer, one_integer);

    while (current != one_fraction)
    {
        if (current > one_fraction)
        {
            result.emplace(result.cbegin(), true);
            current = fraction(current.get_numerator() - current.get_denominator(), current.get_denominator());
        }
        else
        {
            result.emplace(result.cbegin(), false);
            current = fraction(current.get_numerator(), current.get_denominator() - current.get_numerator());
        }
    }
    
    return result;
}

fraction continued_fraction::from_Calkin_Wilf_tree_path(std::vector<bool> const &path)
{
    big_integer const one_integer("1");
    fraction result(one_integer, one_integer);

    for (bool go_right: path)
    {
        if (go_right)
        {
            result = fraction(result.get_denominator() + result.get_numerator(), result.get_denominator());
        }
        else
        {
            result = fraction(result.get_numerator(), result.get_denominator() + result.get_numerator());
        }
    }

    return result;
}