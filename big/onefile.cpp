#ifndef __PROGTEST__



using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

//constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
//constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
//constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
//constexpr unsigned SPREADSHEET_SPEED = 0x08;
//constexpr unsigned SPREADSHEET_PARSER = 0x10;
#endif /* __PROGTEST__ */


#ifndef __PROGTEST__

bool valueMatch(const CValue &r, const CValue &s) {
    if (r.index() != s.index())
        return false;
    if (r.index() == 0)
        return true;
    if (r.index() == 2)
        return std::get<std::string>(r) == std::get<std::string>(s);
    if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
        return true;
    if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
        return (std::get<double>(r) < 0 && std::get<double>(s) < 0) ||
               (std::get<double>(r) > 0 && std::get<double>(s) > 0);
    return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
}

int main() {


    CSpreadsheet x0, x1;
    std::ostringstream oss;
    std::istringstream iss;
    std::string data;
    assert (x0.setCell(CPos("A1"), "10"));
    assert (x0.setCell(CPos("A2"), "20.5"));
    assert (x0.setCell(CPos("A3"), "3e1"));
    assert (x0.setCell(CPos("A4"), "=40"));
    assert (x0.setCell(CPos("A5"), "=5e+1"));
    assert (x0.setCell(CPos("A6"), "raw text with any characters, including a quote \" or a newline\n"));
    assert (x0.setCell(CPos("A7"), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\""));
    assert (valueMatch(x0.getValue(CPos("A1")), CValue(10.0)));
    assert (valueMatch(x0.getValue(CPos("A2")), CValue(20.5)));
    assert (valueMatch(x0.getValue(CPos("A3")), CValue(30.0)));
    auto asd = x0.getValue(CPos("A4"));
    assert (valueMatch(x0.getValue(CPos("A4")), CValue(40.0)));
    assert (valueMatch(x0.getValue(CPos("A5")), CValue(50.0)));
    assert (valueMatch(x0.getValue(CPos("A6")), CValue("raw text with any characters, including a quote \" or a newline\n")));
    assert (valueMatch(x0.getValue(CPos("A7")), CValue("quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++.")));
    assert (valueMatch(x0.getValue(CPos("A8")), CValue()));
    assert (valueMatch(x0.getValue(CPos("AAAA9999")), CValue()));
    assert (x0.setCell(CPos("B1"), "=A1+A2*A3"));
    assert (x0.setCell(CPos("B2"), "= -A1 ^ 2 - A2 / 2   "));
    assert (x0.setCell(CPos("B3"), "= 2 ^ $A$1"));
    assert (x0.setCell(CPos("B4"), "=($A1+A$2)^2"));
    assert (x0.setCell(CPos("B5"), "=B1+B2+B3+B4"));
    assert (x0.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));
    assert (valueMatch(x0.getValue(CPos("B1")), CValue(625.0)));
    assert (valueMatch(x0.getValue(CPos("B2")), CValue(-110.25)));
    assert (valueMatch(x0.getValue(CPos("B3")), CValue(1024.0)));
    assert (valueMatch(x0.getValue(CPos("B4")), CValue(930.25)));
    assert (valueMatch(x0.getValue(CPos("B5")), CValue(2469.0)));
    assert (valueMatch(x0.getValue(CPos("B6")), CValue(4938.0)));
    assert (x0.setCell(CPos("A1"), "12"));
    assert (valueMatch(x0.getValue(CPos("B1")), CValue(627.0)));
    assert (valueMatch(x0.getValue(CPos("B2")), CValue(-154.25)));
    assert (valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x0.getValue(CPos("B4")), CValue(1056.25)));
    assert (valueMatch(x0.getValue(CPos("B5")), CValue(5625.0)));
    assert (valueMatch(x0.getValue(CPos("B6")), CValue(11250.0)));
    x1 = x0;
    assert (x0.setCell(CPos("A2"), "100"));
    assert (x1.setCell(CPos("A2"), "=A3+A5+A4"));
    assert (valueMatch(x0.getValue(CPos("B1")), CValue(3012.0)));
    assert (valueMatch(x0.getValue(CPos("B2")), CValue(-194.0)));
    assert (valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x0.getValue(CPos("B4")), CValue(12544.0)));
    assert (valueMatch(x0.getValue(CPos("B5")), CValue(19458.0)));
    assert (valueMatch(x0.getValue(CPos("B6")), CValue(38916.0)));
    assert (valueMatch(x1.getValue(CPos("B1")), CValue(3612.0)));
    assert (valueMatch(x1.getValue(CPos("B2")), CValue(-204.0)));
    assert (valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x1.getValue(CPos("B4")), CValue(17424.0)));
    assert (valueMatch(x1.getValue(CPos("B5")), CValue(24928.0)));
    assert (valueMatch(x1.getValue(CPos("B6")), CValue(49856.0)));
    oss.clear();
    oss.str("");
    std::ofstream outputFile("output.bin");
    assert (x0.save(oss));
    assert (x0.save(outputFile));
    outputFile.close();
    data = oss.str();
    iss.clear();
    iss.str(data);
    assert (x1.load(iss));
    assert (valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
    assert (valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
    assert (valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
    assert (valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
    assert (valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
    assert (x0.setCell(CPos("A3"), "4e1"));
    assert (valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
    assert (valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
    assert (valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
    assert (valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
    assert (valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
    oss.clear();
    oss.str("");
    assert (x0.save(oss));
    data = oss.str();
    for (size_t i = 0; i < std::min<size_t>(data.length(), 10); i++)
        data[i] ^= 0x5a;
    iss.clear();
    iss.str(data);
    assert (!x1.load(iss));
    assert (x0.setCell(CPos("D0"), "10"));
    assert (x0.setCell(CPos("D1"), "20"));
    assert (x0.setCell(CPos("D2"), "30"));
    assert (x0.setCell(CPos("D3"), "40"));
    assert (x0.setCell(CPos("D4"), "50"));
    assert (x0.setCell(CPos("E0"), "60"));
    assert (x0.setCell(CPos("E1"), "70"));
    assert (x0.setCell(CPos("E2"), "80"));
    assert (x0.setCell(CPos("E3"), "90"));
    assert (x0.setCell(CPos("E4"), "100"));
    assert (x0.setCell(CPos("F10"), "=D0+5"));
    assert (x0.setCell(CPos("F11"), "=$D0+5"));
    assert (x0.setCell(CPos("F12"), "=D$0+5"));
    assert (x0.setCell(CPos("F13"), "=$D$0+5"));
    x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
    assert (valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F14")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G10")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
    assert (valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
    assert (valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
    assert (valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
    x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
    assert (valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F14")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G10")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
    assert (valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
    assert (valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
    assert (valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("H10")), CValue()));
    assert (valueMatch(x0.getValue(CPos("H11")), CValue()));
    assert (valueMatch(x0.getValue(CPos("H12")), CValue()));
    assert (valueMatch(x0.getValue(CPos("H13")), CValue(35.0)));
    assert (valueMatch(x0.getValue(CPos("H14")), CValue()));
    assert (x0.setCell(CPos("F0"), "-27"));
    assert (valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
    x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
    assert (valueMatch(x0.getValue(CPos("H12")), CValue(25.0)));
    assert (valueMatch(x0.getValue(CPos("H13")), CValue(-22.0)));
    assert (valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));

    assert (x0.setCell(CPos("Z15"), "=Z15"));
    assert (valueMatch(x0.getValue(CPos("Z15")), CValue()));
    assert (x0.setCell(CPos("Z16"), "=Z20 + Z21"));
    assert (x0.setCell(CPos("Z21"), "=Z16 + 3"));
    assert (valueMatch(x0.getValue(CPos("Z16")), CValue()));

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
#pragma once

int strToIntCol(std::string & str);

std::string intToStrCol(int n);

std::string addToCol(std::string & str, int n);
#pragma once


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
#pragma once


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
#pragma once

using CValue = std::variant<std::monostate, double, std::string>;

class CTreeNode;
class CSpreadsheet;
class MyExprBuilder;
class CPos;

class CData {
    friend class CSpreadsheet;
public:
    virtual ~CData() = default;

    virtual CValue getCell() const = 0;

    virtual CValue evalCell(CPos &pos, std::unordered_set<CPos> visited) const = 0;

    virtual CData * copy() const = 0;

    virtual CData * deepCopy(CSpreadsheet * spreadsheet) const = 0;

    virtual CData * moveRect(int x, int y) = 0;
};


class CDouble : public CData {
    friend class CSpreadsheet;

public:
    CValue getCell() const override;

    CValue evalCell(CPos &pos, std::unordered_set<CPos> visited) const override;

    CData * copy() const override;

    CData * deepCopy(CSpreadsheet * spreadsheet) const override;

    CData * moveRect(int x, int y) override;

private:
    double m_value;
};


class CString : public CData {
    friend class CSpreadsheet;

public:
    CValue getCell() const override;

    CValue evalCell(CPos &pos, std::unordered_set<CPos> visited) const override;

    CData * copy() const override;

    CData * deepCopy(CSpreadsheet * spreadsheet) const override;

    CData * moveRect(int x, int y) override;

private:
    std::string m_value;
};


class CExpression : public CData {
    friend class CSpreadsheet;

public:

    CExpression(std::string expr, CSpreadsheet * spreadsheet);

    CValue getCell() const override;

    CValue evalCell(CPos &pos, std::unordered_set<CPos> visited) const override;

    CData * copy() const override;

    CData * deepCopy(CSpreadsheet * spreadsheet) const override;

    CData * moveRect(int x, int y) override;

    void moveNode(int x, int y);

private:
    CTreeNode * m_node;
    std::string m_expr;
    CSpreadsheet * m_spreadsheet;
};


#pragma once


using CValue = std::variant<std::monostate, double, std::string>;
class CSpreadsheet;
class CPos;

class CTreeNode {
    friend class MyExprBuilder;
public:
    virtual CValue evalNode(std::unordered_set<CPos> & visited) const = 0;

    virtual CTreeNode * copy() const = 0;

    bool bothDouble(CValue a, CValue b) const;

    bool bothString(CValue a, CValue b) const;

    virtual void moveRect(int x, int y) = 0;

    virtual std::string nodeToString() const = 0;
};


class CLeafNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override {}

    std::string nodeToString() const override;

private:
    CValue m_value;
};


class CRefNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CRefNode(const std::string & originalStr, CSpreadsheet * spreadsheet);

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    std::string m_origStr;
    CPos m_pos;
    bool m_firstFixed;
    bool m_secondFixed;
    CSpreadsheet * m_sheetPtr;
};


class CAddNode : public CTreeNode {

    friend class MyExprBuilder;
public:
    CAddNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};


class CSubNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CSubNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};


class CMulNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CMulNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};


class CDivNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CDivNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};


class CPowNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CPowNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};

class CEqNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CEqNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};

class CNeqNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CNeqNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};

class CLtNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CLtNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};

class CLeNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CLeNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};

class CGtNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CGtNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};

class CGeNode : public CTreeNode {
    friend class MyExprBuilder;
public:
    CGeNode();

    CValue evalNode(std::unordered_set<CPos> & visited) const override;

    CTreeNode * copy() const override;

    void moveRect(int x, int y) override;

    std::string nodeToString() const override;

private:
    CTreeNode * m_left;
    CTreeNode * m_right;
};














#pragma once

using CValue = std::variant<std::monostate, double, std::string>;

class CSpreadsheet;
class CTreeNode;

class MyExprBuilder : public CExprBuilder {
public:
    MyExprBuilder(CSpreadsheet * spreadsheet);

    void opAdd() override;

    void opSub() override;

    void opMul() override;

    void opDiv() override;

    void opPow() override;

    void opNeg() override;

    void opEq() override;

    void opNe() override;

    void opLt() override;

    void opLe() override;

    void opGt() override;

    void opGe() override;

    void valNumber(double val) override;

    void valString(std::string val) override;

    void valReference(std::string val) override;

    void valRange(std::string val) override;

    void funcCall(std::string fnName, int paramCount) override;

    CTreeNode * getRootNode();
private:
    std::stack<CTreeNode *> m_stack;
    CSpreadsheet * m_spreadsheet;
};
#pragma once

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
//#include "CPos.h"
//
//CPos::CPos(std::string_view str) {
//    size_t size = str.size();
//    size_t pos;
//    bool letters = true;
//    std::string row;
//
//    if (std::isdigit(str[0])) {
//        throw std::invalid_argument("Invalid argument\n.");
//    }
//
//    for (size_t i = 0; i < size; i++) {
//        if (letters) {
//            if (!isalpha(str[i])) {
//                if (!std::isdigit(str[i])) {
//                    throw std::invalid_argument("Invalid argument\n.");
//                } else {
//                    letters = false;
//                    pos = i;
//                    row += str[i];
//                }
//            }
//        } else {
//            if (!std::isdigit(str[i])) {
//                throw std::invalid_argument("Invalid argument\n.");
//            } else {
//                row += str[i];
//            }
//        }
//    }
//
//    if (letters) {
//        throw std::invalid_argument("Invalid argument\n.");
//    }
//
//    std::string lower;
//    for (char c : str.substr(0,pos)) {
//        lower += tolower(c);
//    }
//
//    m_column = lower;
//    m_row = std::stoi(row);
//}
//
//



CValue CDouble::getCell() const {
    return m_value;
}

CData * CDouble::copy() const {
    return new CDouble(*this);
}

CData * CDouble::deepCopy(CSpreadsheet * spreadsheet) const {
    return copy();
}

CValue CDouble::evalCell(CPos &pos, std::unordered_set<CPos> visited) const {
    return m_value;
}

CData * CDouble::moveRect(int x, int y) {
    return new CDouble(*this);
}

CValue CString::getCell() const {
    return m_value;
}

CData * CString::copy() const {
    return new CString(*this);
}

CData * CString::deepCopy(CSpreadsheet * spreadsheet) const {
    return copy();
}

CValue CString::evalCell(CPos &pos, std::unordered_set<CPos> visited) const {
    return m_value;
}

CData * CString::moveRect(int x, int y) {
    return new CString(*this);
}


CExpression::CExpression(std::string expr, CSpreadsheet * spreadsheet): m_expr(expr), m_spreadsheet(spreadsheet) {
    MyExprBuilder builder(m_spreadsheet);
    parseExpression(expr, builder);
    m_node = builder.getRootNode();
}

CValue CExpression::getCell() const {
    return m_expr;
}

CData *CExpression::copy() const {
    return new CExpression(*this);
}

CData * CExpression::deepCopy(CSpreadsheet * spreadsheet) const {
    return new CExpression(m_expr, spreadsheet);
}

CValue CExpression::evalCell(CPos & pos, std::unordered_set<CPos> visited) const {
    return m_node->evalNode(visited);
}

void CExpression::moveNode(int x, int y) {
    m_node->moveRect(x, y);
}

CData * CExpression::moveRect(int x, int y) {
    auto * newCell = new CExpression(m_expr, m_spreadsheet);
    newCell->moveNode(x, y);
    newCell->m_expr = '=' + newCell->m_node->nodeToString();
    return newCell;
}

CValue CLeafNode::evalNode(std::unordered_set<CPos> & visited) const {
    return m_value;
}

CTreeNode * CLeafNode::copy() const {
    return new CLeafNode(*this);
}

std::string CLeafNode::nodeToString() const {
    if (std::holds_alternative<double>(m_value)) {
        return std::to_string(std::get<double>(m_value));
    } else if (std::holds_alternative<std::string>(m_value)) {
        return std::get<std::string>(m_value);
    }
}


CRefNode::CRefNode(const std::string & originalStr, CSpreadsheet * spreadsheet) : m_origStr(originalStr), m_sheetPtr(spreadsheet) {
    m_firstFixed = false;
    m_secondFixed = false;
    std::string positionStr;

    int state = 0;
    for (char c : originalStr) {
        switch (state) {
            case 0:
                if (c == '$') {
                    m_firstFixed = true;
                } else {
                    positionStr += c;
                }
                state++;
                break;
            case 1:
                if (c == '$') {
                    m_secondFixed = true;
                    state++;
                } else {
                    positionStr += c;
                }
                break;
            case 2:
                positionStr += c;
                break;
            default:
                break;
        }
    }

    m_pos = CPos(positionStr);
}

CValue CRefNode::evalNode(std::unordered_set<CPos> & visited) const {
    if (!visited.insert(m_pos).second) {
        return {};
    }
    auto ret =  m_sheetPtr->getValueSet(m_pos, visited);
    visited.erase(m_pos);
    return ret;
}

CTreeNode * CRefNode::copy() const {
    return new CRefNode(*this);
}

void CRefNode::moveRect(int x, int y) {
    std::string col = m_pos.getCol();
    int row = m_pos.getRow();

    if (!m_firstFixed) {
        col = addToCol(col, x);
    }
    if (!m_secondFixed) {
        row += y;
    }

    CPos newPos;
    newPos.setCol(col);
    newPos.setRow(row);
    m_pos = newPos;
}

std::string CRefNode::nodeToString() const {
    std::string result;
    if (m_firstFixed) {
        result += '$';
    }
    result += m_pos.getCol();
    if (m_secondFixed) {
        result += '$';
    }
    result += std::to_string(m_pos.getRow());

    return result;
}


CAddNode::CAddNode() : m_left(nullptr), m_right(nullptr) {}

CValue CAddNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return std::get<double>(leftVal) + std::get<double>(rightVal);
    } else if (std::holds_alternative<double>(leftVal) && std::holds_alternative<std::string>(rightVal)) {
        return std::to_string(std::get<double>(leftVal)) + std::get<std::string>(rightVal);
    } else if (std::holds_alternative<std::string>(leftVal) && std::holds_alternative<double>(rightVal)) {
        return std::get<std::string>(leftVal) + std::to_string(std::get<double>(rightVal));
    } else if (bothString(leftVal, rightVal)) {
        return std::get<std::string>(leftVal) + std::get<std::string>(rightVal);
    } else {
        return {};
    }
}

CTreeNode * CAddNode::copy() const {
    return new CAddNode(*this);
}

void CAddNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CAddNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " + ";
    result += m_right->nodeToString();

    return result;
}


CSubNode::CSubNode() : m_left(nullptr), m_right(nullptr) {}

CValue CSubNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (!bothDouble(leftVal, rightVal)) {
        return {};
    }
    return std::get<double>(leftVal) - std::get<double>(rightVal);
}

CTreeNode * CSubNode::copy() const {
    return new CSubNode(*this);
}

void CSubNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CSubNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " - ";
    result += m_right->nodeToString();

    return result;
}


CMulNode::CMulNode() : m_left(nullptr), m_right(nullptr) {}

CValue CMulNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (!bothDouble(leftVal, rightVal)) {
        return {};
    }
    return std::get<double>(leftVal) * std::get<double>(rightVal);
}

CTreeNode * CMulNode::copy() const {
    return new CMulNode(*this);
}

void CMulNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CMulNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " * ";
    result += m_right->nodeToString();

    return result;
}


CDivNode::CDivNode() : m_left(nullptr), m_right(nullptr) {}

CValue CDivNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (!bothDouble(leftVal, rightVal) || std::get<double>(rightVal) == 0.0) {
        return {};
    }

    return std::get<double>(leftVal) / std::get<double>(rightVal);
}

CTreeNode * CDivNode::copy() const {
    return new CDivNode(*this);
}

void CDivNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CDivNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " / ";
    result += m_right->nodeToString();

    return result;
}


CPowNode::CPowNode() : m_left(nullptr), m_right(nullptr) {}

CValue CPowNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (!bothDouble(leftVal, rightVal)) {
        return {};
    }
    return pow(std::get<double>(leftVal), std::get<double>(rightVal));
}

CTreeNode * CPowNode::copy() const {
    return new CPowNode(*this);
}

void CPowNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CPowNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " ^ ";
    result += m_right->nodeToString();

    return result;
}


CEqNode::CEqNode() : m_left(nullptr), m_right(nullptr) {}

CValue CEqNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return (std::get<double>(leftVal) == std::get<double>(rightVal)) ? 1.0 : 0.0;
    } else if (bothString(leftVal, rightVal)) {
        return (std::get<std::string>(leftVal) == std::get<std::string>(rightVal)) ? 1.0 : 0.0;
    } else {
        return {};
    }

}

CTreeNode * CEqNode::copy() const {
    return new CEqNode(*this);
}

void CEqNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CEqNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " == ";
    result += m_right->nodeToString();

    return result;
}


CNeqNode::CNeqNode() : m_left(nullptr), m_right(nullptr) {}

CValue CNeqNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return (std::get<double>(leftVal) != std::get<double>(rightVal)) ? 1.0 : 0.0;
    } else if (bothString(leftVal, rightVal)) {
        return (std::get<std::string>(leftVal) != std::get<std::string>(rightVal)) ? 1.0 : 0.0;
    } else {
        return {};
    }
}

CTreeNode * CNeqNode::copy() const {
    return new CNeqNode(*this);
}

void CNeqNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CNeqNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " != ";
    result += m_right->nodeToString();

    return result;
}


CLtNode::CLtNode() : m_left(nullptr), m_right(nullptr) {}

CValue CLtNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return (std::get<double>(leftVal) < std::get<double>(rightVal)) ? 1.0 : 0.0;
    } else if (bothString(leftVal, rightVal)) {
        return (std::get<std::string>(leftVal) < std::get<std::string>(rightVal)) ? 1.0 : 0.0;
    } else {
        return {};
    }
}

CTreeNode * CLtNode::copy() const {
    return new CLtNode(*this);
}

void CLtNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CLtNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " < ";
    result += m_right->nodeToString();

    return result;
}


CLeNode::CLeNode() : m_left(nullptr), m_right(nullptr) {}

CValue CLeNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return (std::get<double>(leftVal) <= std::get<double>(rightVal)) ? 1.0 : 0.0;
    } else if (bothString(leftVal, rightVal)) {
        return (std::get<std::string>(leftVal) <= std::get<std::string>(rightVal)) ? 1.0 : 0.0;
    } else {
        return {};
    }
}

CTreeNode * CLeNode::copy() const {
    return new CLeNode(*this);
}

void CLeNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CLeNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " <= ";
    result += m_right->nodeToString();

    return result;
}


CGtNode::CGtNode() : m_left(nullptr), m_right(nullptr) {}

CValue CGtNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return (std::get<double>(leftVal) > std::get<double>(rightVal)) ? 1.0 : 0.0;
    } else if (bothString(leftVal, rightVal)) {
        return (std::get<std::string>(leftVal) > std::get<std::string>(rightVal)) ? 1.0 : 0.0;
    } else {
        return {};
    }
}

CTreeNode * CGtNode::copy() const {
    return new CGtNode(*this);
}

void CGtNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CGtNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " > ";
    result += m_right->nodeToString();

    return result;
}


CGeNode::CGeNode() : m_left(nullptr), m_right(nullptr) {}

CValue CGeNode::evalNode(std::unordered_set<CPos> & visited) const {
    auto leftVal = m_left->evalNode(visited);
    auto rightVal = m_right->evalNode(visited);

    if (bothDouble(leftVal, rightVal)) {
        return (std::get<double>(leftVal) >= std::get<double>(rightVal)) ? 1.0 : 0.0;
    } else if (bothString(leftVal, rightVal)) {
        return (std::get<std::string>(leftVal) >= std::get<std::string>(rightVal)) ? 1.0 : 0.0;
    } else {
        return {};
    }
}

CTreeNode * CGeNode::copy() const {
    return new CGeNode(*this);
}

void CGeNode::moveRect(int x, int y) {
    m_left->moveRect(x, y);
    m_right->moveRect(x, y);
}

std::string CGeNode::nodeToString() const {
    std::string result;
    result += m_left->nodeToString();
    result += " >= ";
    result += m_right->nodeToString();

    return result;
}


bool CTreeNode::bothDouble(CValue a, CValue b) const {
    return (std::holds_alternative<double>(a) && std::holds_alternative<double>(b));
}

bool CTreeNode::bothString(CValue a, CValue b) const {
    return (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b));
}

MyExprBuilder::MyExprBuilder(CSpreadsheet *spreadsheet): m_spreadsheet(spreadsheet) {}

void MyExprBuilder::opAdd() {
    CAddNode * node = new CAddNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opSub() {
    CSubNode * node = new CSubNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opMul() {
    CMulNode * node = new CMulNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opDiv() {
    CDivNode * node = new CDivNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opPow() {
    CPowNode * node = new CPowNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opNeg() {
    CLeafNode * leaf = new CLeafNode;
    leaf->m_value = -1.0;
    CMulNode * node = new CMulNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = leaf;
    m_stack.push(node);
}

void MyExprBuilder::opEq() {
    CEqNode * node = new CEqNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opNe() {
    CNeqNode * node = new CNeqNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opLt() {
    CLtNode * node = new CLtNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opLe() {
    CLeNode * node = new CLeNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opGt() {
    CGtNode * node = new CGtNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::opGe() {
    CGeNode * node = new CGeNode;
    node->m_right = m_stack.top();
    m_stack.pop();
    node->m_left = m_stack.top();
    m_stack.pop();
    m_stack.push(node);
}

void MyExprBuilder::valNumber(double val) {
    CLeafNode * node = new CLeafNode;
    node->m_value = val;
    m_stack.push(node);
}

void MyExprBuilder::valString(std::string val) {
    CLeafNode * node = new CLeafNode;
    node->m_value = val;
    m_stack.push(node);
}

void MyExprBuilder::valReference(std::string val) {
    CRefNode * node = new CRefNode(val, m_spreadsheet);
    m_stack.push(node);
}

void MyExprBuilder::valRange(std::string val) {

}

void MyExprBuilder::funcCall(std::string fnName, int paramCount) {

}

CTreeNode * MyExprBuilder::getRootNode() {
    // todo co kdyz na stacku je nic nebo vic jak jedna vec
    return m_stack.top();
}



CSpreadsheet::CSpreadsheet() = default;

CSpreadsheet::CSpreadsheet(const CSpreadsheet & other) {
    for (const auto & cell : other.m_sheet) {
        CPos newPos(cell.first);
        CData * newData = cell.second->copy();
        m_sheet[newPos] = newData;
    }
}

CSpreadsheet &CSpreadsheet::operator=(const CSpreadsheet &other) {
    if (this != &other) {
        for (auto & cell : m_sheet) {
            delete cell.second;
        }

        m_sheet.clear();

        for (const auto & cell : other.m_sheet) {
            CPos newPos(cell.first);
            CData * newData = cell.second->deepCopy(this);
            m_sheet[newPos] = newData;
        }
    }

    return *this;
}

CSpreadsheet::~CSpreadsheet() {
    for (auto & cell : m_sheet) {
        delete cell.second;
    }
    m_sheet.clear();
}

bool CSpreadsheet::load(std::istream & is) {
    if (is.fail()) {
        return false;
    }

    int numOfCells = readInt(is);
    int i = 0;
    int colLen;
    size_t controlSum = 0;

    try {
        while (i < numOfCells) {
            colLen = readInt(is);
            if (colLen == -1) {
                break;
            }
            std::string cellCol = readString(is, colLen);

            controlSum += cellCol.size();
            for (const char & c : cellCol) {
                controlSum += c;
            }

            int rowLen = readInt(is);
            if (rowLen == -1) {
                break;
            }
            std::string cellRow = readString(is, rowLen);

            controlSum += cellRow.size();
            for (const char & c : cellRow) {
                controlSum += c;
            }

            int contentLen = readInt(is);
            if (contentLen == -1) {
                break;
            }
            std::string content = readString(is, contentLen);

            controlSum += content.size();
            for (const char & c : content) {
                controlSum += c;
            }

            CPos cell(cellCol + cellRow);
            setCell(cell, content);
            i++;
        }

    } catch (const std::exception & e) {
        return false;
    }

    if (readInt(is) != controlSum) {
        return false;
    }

    return true;
}

bool CSpreadsheet::save(std::ostream & os) const {
    size_t numOfCells = m_sheet.size();
    os.write((char *)&numOfCells, sizeof(int));

    size_t controlSum = 0;
    for (const auto & cell : m_sheet) {
        strToBin(os, cell.first.getCol(), controlSum);
        strToBin(os, std::to_string(cell.first.getRow()), controlSum);
        std::string cellValue;
        if (std::holds_alternative<double>(cell.second->getCell())) {
            cellValue = std::to_string(std::get<double>(cell.second->getCell()));
            strToBin(os, cellValue, controlSum);
        } else {
            strToBin(os, std::get<std::string>(cell.second->getCell()), controlSum);
        }
    }
    os.write((char *)&controlSum, sizeof(int));

    return true;
}

bool CSpreadsheet::setCell(CPos pos, std::string contents) {
    CValue value = convertToCValue(contents);
    if (std::holds_alternative<std::monostate>(value)) {
        return false;
    }

    if (std::holds_alternative<double>(value)) {
        CDouble dbl;
        dbl.m_value = std::get<double>(value);
        m_sheet[pos] = dbl.copy();
        return true;

    } else if (std::holds_alternative<std::string>(value)) {
        std::string strValue = std::get<std::string>(value);
        if (strValue[0] == '=') {
            CExpression expr(strValue, this);
            MyExprBuilder builder(this);
            try {
                parseExpression(strValue, builder);
            } catch (const std::exception & e) {
                return false;
            }
            m_sheet[pos] = expr.copy();
            return true;

        } else {
            CString str;
            str.m_value = strValue;
            m_sheet[pos] = str.copy();
            return true;
        }
    }
}

CValue CSpreadsheet::getValue(CPos pos) {
    auto it = m_sheet.find(pos);

    std::unordered_set<CPos> visited;
    visited.insert(pos);

    if (it != m_sheet.end()) {
        return getValueSet(pos, visited);
    }
    return {};
}

CValue CSpreadsheet::getValueSet(CPos pos, std::unordered_set<CPos> visited) {
    auto it = m_sheet.find(pos);

    if (it != m_sheet.end()) {
        return it->second->evalCell(pos, visited);
    }

    return {};
}

void CSpreadsheet::copyRect(CPos dst, CPos src, int w, int h) {
    int srcX = strToIntCol(src.m_column);
    int dstX = strToIntCol(dst.m_column);
    std::pair<int, int> moveBy = {dstX - srcX, dst.getRow() - src.getRow()};

    std::map<CPos, CData *> tmpMap;

    for (const auto & cell : m_sheet) {
        if (cell.first.getCol() >= src.getCol() && cell.first.getCol() < addToCol(src.m_column, w) && cell.first.getRow() >= src.getRow() && cell.first.getRow() < src.getRow() + h) {
            CPos newPos;
            auto sourceStr = cell.first.getCol();
            newPos.m_column = addToCol(sourceStr, moveBy.first);
            newPos.m_row = cell.first.getRow() + moveBy.second;
            tmpMap[newPos] = cell.second->moveRect(moveBy.first, moveBy.second);
        }
    }

    for (const auto & tmpCell : tmpMap) {
        m_sheet[tmpCell.first] = tmpCell.second;
    }

}

CValue CSpreadsheet::convertToCValue(const std::string & contents) {
    if (contents.empty()) {
        return std::monostate();
    }

    std::string value = contents;
    try {
        size_t pos;
        double number = std::stod(contents, &pos);
        if (pos == contents.size()) {
            return number;
        }
    } catch (const std::invalid_argument &) {}

    return value;
}

void CSpreadsheet::strToBin(std::ostream & os, const std::string & str, size_t &controlSum) const {
    size_t strSize = str.size();
    os.write((char *)&strSize, sizeof(int));
    controlSum += strSize;

    for (const char & c : str) {
        os.write((char *) &c, sizeof(char));
        controlSum += c;
    }
}

std::string CSpreadsheet::readString(std::istream & is, int numberOfChars) const {
    if (is.eof()) {
        throw std::exception();
    }
    std::vector<char> charVector(numberOfChars);
    is.read(charVector.data(), numberOfChars);
    if (!is) {
        throw std::exception();
    }

    std::string str(charVector.begin(), charVector.end());
    return str;
}

size_t CSpreadsheet::readInt(std::istream & is) const {
    if (is.eof()) {
        return -1;
    }
    int intValue;
    is.read(reinterpret_cast<char*>(&intValue), sizeof(int));
    if (!is) {
        return -1;
    }

    return intValue;
}

