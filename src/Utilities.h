#ifndef UTILITIES_H
#define UTILITIES_H
#pragma once

#include <string>

static std::string ToHex1(char val)
{
    char buf[5];
    snprintf(buf, 5, "0x%02x", val);
    return std::string(buf);
}

static std::string ToHex2(int val)
{
    char buf[7];
    snprintf(buf, 7, "0x%04x", val);
    return std::string(buf);
}

static std::string ToHex4(int val)
{
    char buf[11];
    snprintf(buf, 11, "0x%08x", val);
    return std::string(buf);
}

#endif // UTILITIES_H