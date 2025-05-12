#pragma once
#include <iostream>
#include <variant>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "CPos.h"
#include "CData.h"
#include "expression.h"
#include "utils.h"

using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

class CPos;

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;

class CData;
class CSpreadsheet {
    friend class MyExprBuilder;
    friend class CData;
    friend class CDouble;
    friend class CString;
    friend class CExpression;
public:
    static unsigned capabilities() {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FUNCTIONS | SPREADSHEET_FILE_IO | SPREADSHEET_SPEED;
    }

    CSpreadsheet();

    CSpreadsheet(const CSpreadsheet & other);

    CSpreadsheet & operator=(const CSpreadsheet & other);

    ~CSpreadsheet();

    bool load(std::istream & is);

    bool save(std::ostream & os) const;

    bool setCell(CPos pos, std::string contents);

    CValue getValue(CPos pos);

    CValue getValueSet(CPos pos, std::unordered_set<CPos> visited);

    void copyRect(CPos dst, CPos src, int w = 1, int h = 1);

private:
    std::map<CPos, CData *> m_sheet;

    CValue convertToCValue(const std::string & contents);

    void strToBin(std::ostream & os, const std::string & str, size_t & controlSum) const;

    std::string readString(std::istream & is, int numberOfChars) const;

    size_t readInt(std::istream & is) const;

};
