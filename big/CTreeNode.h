#pragma once
#include <iostream>
#include <variant>
#include <cmath>
#include <set>
#include <unordered_set>
#include "CPos.h"
#include "MyExprBuilder.h"
#include "CSpreadsheet.h"


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














