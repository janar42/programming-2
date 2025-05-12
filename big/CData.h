#pragma once
#include <iostream>
#include <variant>
#include <unordered_set>
#include "CPos.h"
#include "MyExprBuilder.h"

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


