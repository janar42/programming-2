#include "CData.h"


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
