#pragma once
#include "utils.h"


int strToIntCol(std::string & str) {
    int result = 0;
    for (char c : str) {
        result *= 26;
        result += c - 'a' + 1;
    }
    return result;
}

std::string intToStrCol(int n) {
    std::string result;
    while (n > 0) {
        int remainder = (n - 1) % 26;
        result = char('a' + remainder) + result;
        n = (n - 1) / 26;
    }
    return result;
}

std::string addToCol(std::string  &str, int n) {
    int col = strToIntCol(str);
    col += n;
    return intToStrCol(col);
}
