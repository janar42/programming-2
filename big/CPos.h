#pragma once
#include <iostream>
#include <cctype>


class CPos {
public:
    //friend class CSpreadsheet;

    CPos(std::string_view str) {
        size_t size = str.size();
        size_t pos;
        bool letters = true;
        std::string row;

        if (std::isdigit(str[0])) {
            throw std::invalid_argument("Invalid argument\n.");
        }

        for (size_t i = 0; i < size; i++) {
            if (letters) {
                if (!isalpha(str[i])) {
                    if (!std::isdigit(str[i])) {
                        throw std::invalid_argument("Invalid argument\n.");
                    } else {
                        letters = false;
                        pos = i;
                        row += str[i];
                    }
                }
            } else {
                if (!std::isdigit(str[i])) {
                    throw std::invalid_argument("Invalid argument\n.");
                } else {
                    row += str[i];
                }
            }
        }

        if (letters) {
            throw std::invalid_argument("Invalid argument\n.");
        }

        std::string lower;
        for (char c : str.substr(0,pos)) {
            lower += tolower(c);
        }

        m_column = lower;
        m_row = std::stoi(row);
    }

    CPos() : m_column(""), m_row(0) {}

    CPos(std::string & str, int n) : m_column(str), m_row(n) {}

    bool operator==(const CPos & other) const {
        return m_column == other.m_column && m_row == other.m_row;
    }

    bool operator<(const CPos & other) const {
        return m_column < other.m_column || (m_column == other.m_column && m_row < other.m_row);
    }

    std::string getCol() const {
        return m_column;
    }

    int getRow() const {
        return m_row;
    }

    void setCol(std::string & col) {
        m_column = col;
    }

    void setRow(int row) {
        m_row = row;
    }

    friend struct std::hash<CPos>;

    std::string m_column;
    int m_row;
private:

};

namespace std {
    template<>
    struct hash<CPos> {
        size_t operator()(const CPos & pos) const {
            return hash<string>()(pos.m_column) ^ hash<int>()(pos.m_row);
        }
    };
}