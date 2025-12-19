#ifndef STDLIB_H
#define STDLIB_H
#pragma once

#include <set>
#include <string>
#include <random>
#include <iostream>


double __rand_impl()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dist(0.0, 1.0);
    return dist(gen);
}

int64_t __rand_range_impl(int64_t lhs, int64_t rhs)
{
    if (lhs == rhs) return lhs;
    if (lhs > rhs) {
        int64_t temp = lhs;
        lhs = rhs;
        rhs = temp;
    }
    // todo - go back and profile
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(lhs, rhs);
    return dist(gen);
}

std::string console_input()
{
    char p[256];
    memset(p, 0, 256);
    std::cin.getline(p, 255);
    return std::string(p);
}



#endif // STDLIB_H