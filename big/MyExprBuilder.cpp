#include "MyExprBuilder.h"

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

