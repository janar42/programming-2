#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <stdexcept>

#endif /* __PROGTEST__ */

struct Node {
    std::shared_ptr<char> m_str;
    unsigned int m_offset;
    size_t m_len;
    Node *m_next;
};

struct NodesInfo {
    Node *first;
    Node *last;
};

class CPatchStr {
private:
    Node *m_node;
    size_t m_size;

public:
    CPatchStr() {
        m_node = new Node {nullptr, 0, 0, nullptr};
        m_size = 0;
    }

    CPatchStr(const char *str) {
        size_t len = strlen(str);

        char *newString = new char[len + 1];
        std::strcpy(newString, str);

        m_node = new Node{std::shared_ptr<char>(newString),  0, len, nullptr};
        m_size = len;
    }

    CPatchStr(const CPatchStr &other) {
        m_size = other.m_size;
        if (other.m_node) {
            NodesInfo newNodes = copyNodes(other.m_node);
            m_node = newNodes.first;
        } else {
            m_node = nullptr;
        }
    }

    void destroyList(Node *start) {
        Node *tmp = start;
        while (tmp) {
            Node *next = tmp->m_next;
            tmp->m_str.reset();
            delete tmp;
            tmp = next;
        }
    }

    ~CPatchStr() {
        destroyList(m_node);
    }

    CPatchStr &operator=(const CPatchStr &other) {
        if (this != &other) {
            NodesInfo newNodes = copyNodes(other.m_node);
            destroyList(m_node);
            m_node = newNodes.first;
            m_size = other.m_size;
        }
        return *this;
    }

    CPatchStr & append(const CPatchStr &src) {
        Node *myEnd = this->m_node;
        while (myEnd->m_next != nullptr) {
            myEnd = myEnd->m_next;
        }

        NodesInfo newNodes = copyNodes(src.m_node);

        myEnd->m_next = newNodes.first;
        this->m_size += src.m_size;

        return *this;
    }

    NodesInfo copyNodes(Node *src) {
        Node * newTailStart = nullptr;
        Node * newTailEnd = nullptr;

        while (src != nullptr) {
            Node *newNode = new Node;

            newNode->m_offset = src->m_offset;
            newNode->m_next = nullptr;
            newNode->m_len = src->m_len;
            std::shared_ptr<char> ptr2(src->m_str);
            newNode->m_str = ptr2;

            if (newTailStart == nullptr) {
                newTailStart = newNode;
                newTailEnd = newNode;
            } else {
                newTailEnd->m_next = newNode;
                newTailEnd = newNode;
            }
            src = src->m_next;
        }

        return NodesInfo{newTailStart, newTailEnd};
    }

    CPatchStr subStr(size_t from, size_t len) const {
        if (from + len > m_size || from > m_size) {
            throw std::out_of_range("Neplatny rozsah");
        }

        if (len == 0) {
            CPatchStr result;
            return result;
        }

        //najiti uzlu, kde zacina substring
        Node * tmp = m_node;
        size_t position = 0;
        while (position < from) {
            if (position + tmp->m_len > from) {
                break;
            }
            position += tmp->m_len;
            tmp = tmp->m_next;
        }

        // kopirovani prvniho uzlu
        size_t startOffset = from - position;
        CPatchStr result;
        std::shared_ptr<char> ptr2(tmp->m_str);
        result.m_node->m_str = ptr2;
        result.m_node->m_offset = startOffset;
        result.m_node->m_next = nullptr;

        // spocitej delku prvniho nodu
        size_t maxLength = (tmp->m_len - startOffset);
        result.m_node->m_len = (len >= maxLength) ? maxLength : len;

        result.m_size = result.m_node->m_len;

        Node *tail = result.m_node;
        // kopirovani zbylych uzlu
        while (result.m_size < len) {
            tmp = tmp->m_next;
            Node *newNode = new Node;
            newNode->m_offset = tmp->m_offset;
            newNode->m_next = nullptr;
            std::shared_ptr<char> ptr3(tmp->m_str);
            newNode->m_str = ptr3;

            tail->m_next = newNode;
            tail = newNode;

            maxLength = tmp->m_len;
            newNode->m_len = (len - result.m_size >= maxLength) ? maxLength : len - result.m_size;
            result.m_size += newNode->m_len;
        }

        return result;
    }

    CPatchStr & insert(size_t pos, const CPatchStr &src) {
        if (pos > this->m_size) {
            throw std::out_of_range("Neplatna pozice");
        } else if (pos == this->m_size) {
            this->append(src);
        }

        if(pos == 0) {
            NodesInfo newNodes = copyNodes(src.m_node);
            newNodes.last->m_next = this->m_node;
            this->m_node = newNodes.first;
        }

        Node * tmp = m_node;
        Node * previous;
        size_t findPos = 0;
        while (findPos < pos) {
            if (findPos + tmp->m_len > pos) {
                break;
            }
            findPos += tmp->m_len;
            previous = tmp;
            tmp = tmp->m_next;
        }

        Node * left = nullptr;
        Node * right = nullptr;

        if (pos == findPos) { //pozice je na konci nejakeho nodu
            left = previous;
            right = tmp;
        } else { //pozice je uvnitr nodu
            size_t leftPartSize = pos - findPos;
            size_t previousLength = tmp->m_len;
            left = tmp;
            left->m_len = leftPartSize;

            right = new Node();
            std::shared_ptr<char> ptr3(left->m_str);
            right->m_str = ptr3;
            right->m_offset = left->m_len + left->m_offset;
            right->m_next = left->m_next;
            right->m_len = previousLength - leftPartSize;

            left->m_next = right;
        }

        NodesInfo newNodes = copyNodes(src.m_node);
        left->m_next = newNodes.first;
        newNodes.last->m_next = right;

        m_size += src.m_size;
        return *this;
    }

    CPatchStr & remove(size_t from, size_t len) {
        if (from + len > this->m_size || from > m_size) {
            throw std::out_of_range("Neplatny rozsah");
        } else if (len == 0) {
            return *this;
        }

        //hledani nodu, kde zacina remove
        Node * tmp = m_node;
        Node * previous = nullptr;
        size_t findPos = 0;
        while (findPos < from) {
            if (findPos + tmp->m_len > from) {
                break;
            }
            findPos += tmp->m_len;
            previous = tmp;
            tmp = tmp->m_next;
        }

        Node * left;
        Node * right;

        if (findPos == from) { //pozice je na konci nodu
            left = previous;
            right = tmp;
        } else { //pozice je uprostred nodu
            size_t leftPartSize = from - findPos;
            size_t previousLength = tmp->m_len;
            left = tmp;
            left->m_len = leftPartSize;

            right = new Node();
            std::shared_ptr<char> ptr3(left->m_str);
            right->m_str = ptr3;
            right->m_offset = left->m_len + left->m_offset;
            right->m_next = left->m_next;
            right->m_len = previousLength - leftPartSize;
            left->m_next = right;
        }

        //hledani nodu, kde konci remove a zaroven offsetu v novem nodu
        size_t toRemove = len;
        while (toRemove > 0 && right != nullptr) {
            if (right->m_len == toRemove) {
                // smazat cely uzel
                if(left == nullptr) {
                    m_node = right;
                } else {
                    left->m_next = right->m_next;
                }
                right->m_str.reset();
                delete right;
                break;
            } else if (right->m_len < toRemove) {
                // smazat cely uzel a neco navic
                toRemove -= right->m_len;
                right->m_str.reset();
                Node * tmp4 = right;
                right = right->m_next;
                delete tmp4;
            } else {
                // posunout offset
                right->m_offset += toRemove;
                right->m_len -= toRemove;
                if(left == nullptr) {
                    m_node = right;
                } else {
                    left->m_next = right;
                }
                break;
            }
        }

        m_size -= len;
        return *this;
    }

    char *toStr() const {
        char *string = new char[this->m_size + 1];
        Node *tmp = this->m_node;
        int j = 0;
        while (tmp != nullptr) {
            for (unsigned int i = tmp->m_offset; i < tmp->m_len + tmp->m_offset; i++) {
                string[j] = tmp->m_str.get()[i];
                j++;
            }
            tmp = tmp->m_next;
        }
        string[j] = '\0';
        return string;
    }

};

#ifndef __PROGTEST__

bool stringMatch(char *str, const char *expected) {
    bool res = std::strcmp(str, expected) == 0;
    delete[] str;
    return res;
}

int main() {
    char tmpStr[100];

    CPatchStr a("test");
    assert (stringMatch(a.toStr(), "test"));
    std::strncpy(tmpStr, " da", sizeof(tmpStr) - 1);
    a.append(tmpStr);
    assert (stringMatch(a.toStr(), "test da"));
    std::strncpy(tmpStr, "ta", sizeof(tmpStr) - 1);
    a.append(tmpStr);
    assert (stringMatch(a.toStr(), "test data"));
    std::strncpy(tmpStr, "foo text", sizeof(tmpStr) - 1);
    CPatchStr b(tmpStr);
    assert (stringMatch(b.toStr(), "foo text"));
    CPatchStr c(a);
    assert (stringMatch(c.toStr(), "test data"));
    CPatchStr d(a.subStr(3, 5));
    assert (stringMatch(d.toStr(), "t dat"));
    d.append(b);
    assert (stringMatch(d.toStr(), "t datfoo text"));
    d.append( b.subStr(3, 4));
    assert (stringMatch(d.toStr(), "t datfoo text tex"));
    c.append(d);
    assert (stringMatch(c.toStr(), "test datat datfoo text tex"));
    c.append(c);
    assert (stringMatch(c.toStr(), "test datat datfoo text textest datat datfoo text tex"));
    d.insert(2, c.subStr(6, 9));
    assert (stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
    b = "abcdefgh";
    CPatchStr asd = b.subStr(2, 6);
    assert (stringMatch(b.toStr(), "abcdefgh"));
    assert (stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
    assert (stringMatch(d.subStr(4, 8).toStr(), "at datfd"));
    assert (stringMatch(b.subStr(2, 6).toStr(), "cdefgh"));
    try {
        b.subStr(2, 7).toStr();
        assert ("Exception not thrown" == nullptr);
    }
    catch (const std::out_of_range &e) {
    }
    catch (...) {
        assert ("Invalid exception thrown" == nullptr);
    }
    a.remove(3, 5);

    assert (stringMatch(a.toStr(), "tesa"));

    CPatchStr asd2("asdasdasd");
    asd2.remove(0, 3);
    assert (stringMatch(asd2.toStr(), "asdasd"));
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
