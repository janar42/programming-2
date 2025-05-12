#pragma once
#include <iostream>
#include <cctype>
#include <stack>
#include <variant>
#include "expression.h"
#include "CSpreadsheet.h"
#include "CTreeNode.h"

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
