#include <iostream>
#include <continued_fraction.h>

int main(int argc, char **argv)
{
    fraction frac = continued_fraction::from_continued_fraction_representation(std::vector<big_integer> { big_integer("4"), big_integer("6"), big_integer("1"), big_integer("2") });
    auto vec = continued_fraction::to_convergents_series(frac);
    // frac = continued_fraction::from_Calkin_Wilf_tree_path(vec);
    std::cout << frac << std::endl;
    for (auto const &elem: vec)
    {
        std::cout << elem << std::endl;
    }
    return 0;
} 