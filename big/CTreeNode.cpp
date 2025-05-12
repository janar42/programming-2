#include "CTreeNode.h"
#include "utils.h"

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
