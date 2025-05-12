#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <compare>
#include <stdexcept>

#endif /* __PROGTEST__ */

bool leftOneBigger(const std::vector<uint8_t> & left, const std::vector<uint8_t> & right) { //compares absolute values of two CBigInts
    if (left.size() > right.size()) {
        return true;
    } else if (left.size() < right.size()) {
        return false;
    }
    if (left == right) {
        return false;
    }

    auto leftTt = left.rbegin();
    auto rightIt = right.rbegin();
    while (leftTt != left.rend()) {
        if ((*leftTt) != (*rightIt)) {
            return ((*leftTt) > (*rightIt));
        }
        leftTt++;
        rightIt++;
    }
    return false;
}

//numbers in vectors are stored backwards
class CBigInt {
public:
    CBigInt() {
        m_number.push_back(0);
        m_sign = true;
    }

    CBigInt(const int num) {
        if (num == 0) {
            m_number.push_back(0);
            m_sign = true;
        } else {
            m_sign = (num >= 0);
            int tmp = abs(num);
            while (tmp != 0) {
                m_number.push_back(tmp % 10);
                tmp /= 10;
            }
        }
    }

    CBigInt(const char *str) {
        std::string newStr(str);
        if (newStr[0] == '-') {
            newStr.erase(newStr.begin());
            m_sign = false;
        } else {
            m_sign = true;
        }

        for (char i : newStr) {
            if (!isdigit(i)) {
                throw std::invalid_argument("Nespravny vstup\n.");
            }
        }

        auto index = newStr.find_first_not_of('0');
        if (index == newStr.size() - 1) {
            m_number.push_back(0);
        } else {
            newStr.erase(0, index);
            for (auto it = newStr.rbegin(); it != newStr.rend(); it++) {
                m_number.push_back((*it) - '0');
            }
        }
    }
/*
    CBigInt operator=(const CBigInt & c) {
        this->m_number = c.m_number;
        this->m_sign = c.m_sign;
        return *this;
    }*/

    friend CBigInt operator+(const CBigInt &c1, const CBigInt &c2) {
        CBigInt result;
        if ((c1.m_sign && c2.m_sign) || (!c1.m_sign && !c2.m_sign)) {
            return result.addVectors(c1, c2);
        } else if (c1.m_sign && !c2.m_sign) {
            return result.subVectors(c2, c1);
        } else if (!c1.m_sign && c2.m_sign) {
            return result.subVectors(c1, c2);
        }
        return result;
    }

    friend void operator+=(CBigInt & c1, const CBigInt & c2) {
        c1 = (c1 + c2);
    }

    friend CBigInt operator*(const CBigInt &c1, const CBigInt &c2) {
        if (c1 == 0 || c2 == 0) {
            return 0;
        } else if (c1 == 1) {
            return c2;
        } else if (c2 == 1) {
            return c1;
        }

        CBigInt result;
        result.m_number.pop_back();
        result.m_sign = (c1.m_sign == c2.m_sign);
        result.m_number = std::vector<uint8_t>(c1.m_number.size() + c2.m_number.size(), 0);
        auto c1Size = c1.m_number.size();
        auto c2Size = c2.m_number.size();
        int res;
        int carry;

        for (size_t c1It = 0; c1It < c1Size; c1It++) {
            carry = 0;
            for (size_t c2It = 0; c2It < c2Size; c2It++) {
                res = c1.m_number[c1It] * c2.m_number[c2It] + carry + result.m_number[c1It + c2It];
                result.m_number[c1It + c2It] = (res % 10);
                carry = res / 10;
            }
            result.m_number[c2Size + c1It] += carry;
        }

        result.eraseZeros();
        return result;
    }

    friend void operator*=(CBigInt & c1, const CBigInt & c2) {
        c1 = (c1 * c2);
    }

    friend bool operator==(const CBigInt &c1, const CBigInt &c2) {
        return (c1.m_sign == c2.m_sign && c1.m_number == c2.m_number);
    }

    friend bool operator!=(const CBigInt &c1, const CBigInt &c2) {
        return !(c1 == c2);
    }

    friend bool operator<(const CBigInt &c1, const CBigInt &c2) {
        if (c1 == c2) {
            return false;
        }
        if (!c1.m_sign && c2.m_sign) {
            return true;
        } else if (c1.m_sign && !c2.m_sign) {
            return false;
        } else if (!c1.m_sign && !c2.m_sign) {
            return leftOneBigger(c1.m_number, c2.m_number);
        } else if (c1.m_sign && c2.m_sign) {
            return !leftOneBigger(c1.m_number, c2.m_number);
        }
        return false;
    }

    friend bool operator<=(const CBigInt &c1, const CBigInt &c2) {
        return (c1 < c2 || c1 == c2);
    }

    friend bool operator>(const CBigInt &c1, const CBigInt &c2) {
        if (c1 == c2) {
            return false;
        }
        return !(c1 < c2);
    }

    friend bool operator>=(const CBigInt &c1, const CBigInt &c2) {
        return (c1 > c2 || c1 == c2);
    }

    friend std::ostream & operator<<(std::ostream & os, CBigInt c) {
        os << ((c.m_sign) ? "" : "-");
        auto it = c.m_number.rbegin();
        while (it != c.m_number.rend()) {
            os << (int)(*it);  //TODO ???????????????
            it++;
        }
        return os;
    }

    friend std::istream & operator>>(std::istream & is, CBigInt & c) {
        std::string inStr;
        is >> inStr;

        bool sign = true;
        int start = 0;
        if (inStr[0] == '-') {
            if (!isdigit(inStr[1])) {
                is.setstate(std::ios::failbit);
                return is;
            }
            sign = false;
            start = 1;
        } else if (!isdigit(inStr[0])) {
            is.setstate(std::ios::failbit);
            return is;
        }

        size_t i;
        size_t size = inStr.size();
        for (i = start; i < size; i++) {
            if (!isdigit(inStr[i])) {
                break;
            }
        }

        std::string str;
        for (size_t j = 0; j < i; j++) {
            str.push_back(inStr[j]);
        }

        c = CBigInt(str.c_str());
        c.m_sign = sign;
        return is;
    }

private:

    CBigInt addVectors(const CBigInt &c1, const CBigInt &c2) const {
        if (c1 == 0) {
            return c2;
        } else if (c2 == 0) {
            return c1;
        }

        auto c1It = c1.m_number.begin();
        auto c2It = c2.m_number.begin();
        CBigInt result;
        result.m_number.pop_back();
        result.m_sign = c1.m_sign;
        int carry = 0;

        while (c1It != c1.m_number.end() || c2It != c2.m_number.end()) {
            int res;
            if (c1It != c1.m_number.end() && c2It != c2.m_number.end()) {
                res = (*c1It) + (*c2It) + carry;
                if (res > 9) {
                    carry = 1;
                    res %= 10;
                } else {
                    carry = 0;
                }
                result.m_number.push_back(res);
                c1It++;
                c2It++;
            } else if (c1It == c1.m_number.end() && c2It != c2.m_number.end()) {
                res = (*c2It) + carry;
                if (res > 9) {
                    carry = 1;
                    res %= 10;
                } else {
                    carry = 0;
                }
                result.m_number.push_back(res);
                c2It++;
            } else if (c1It != c1.m_number.end() && c2It == c2.m_number.end()) {
                res = (*c1It) + carry;
                if (res > 9) {
                    carry = 1;
                    res %= 10;
                } else {
                    carry = 0;
                }
                result.m_number.push_back(res);
                c1It++;
            }
        }
        if (carry) {
            result.m_number.push_back(carry);
        }
        return result;
    }

    CBigInt subVectors(const CBigInt & negative, const CBigInt & positive) {
        if (negative == 0) {
            return positive;
        } else if (positive == 0) {
            return negative;
        }

        CBigInt result;
        result.m_number.pop_back();
        if (negative.m_number == positive.m_number) {
            result.m_number.push_back(0);
            result.m_sign = true;
            return result;
        }
        result.m_sign = !(negative.m_number > positive.m_number);
        std::vector<uint8_t> smaller;
        std::vector<uint8_t> bigger;
        if (leftOneBigger(negative.m_number, positive.m_number)) {
            smaller = positive.m_number;
            bigger = negative.m_number;
        } else {
            smaller = negative.m_number;
            bigger = positive.m_number;
        }
        while (smaller.size() != bigger.size()) {
            smaller.push_back(0);
        }
        int carry = 0;
        auto smIt = smaller.begin();
        auto biIt = bigger.begin();
        int res;

        while (biIt != bigger.end()) {
            if ((*biIt) < (*smIt)) {
                res = 10 + (*biIt) - (*smIt) - carry;
                carry = 1;
            } else {
                res = (*biIt) - (*smIt - carry);
                carry = 0;
            }
            result.m_number.push_back(res);
            biIt++;
            smIt++;
        }
        result.eraseZeros();
        return result;
    }

    void eraseZeros() {
        auto it = this->m_number.rbegin();
        while (this->m_number.size() > 1) {
            if ((*it) != 0) {
                break;
            } else if ((*it) == 0) {
                this->m_number.pop_back();
            }
            it++;
        }
    }

    std::vector<uint8_t> m_number;
    bool m_sign;
};

#ifndef __PROGTEST__


static bool equal(const CBigInt &x, const char val[]) {
    std::ostringstream oss;
    oss << x;
    return oss.str() == val;
}

int main() {
    CBigInt a, b;
    std::istringstream is;
    a = 10;
    a += 20;
    assert (equal(a, "30"));
    a *= 5;
    assert (equal(a, "150"));
    b = a + 3;
    assert (equal(b, "153"));
    b = a * 7;
    assert (equal(b, "1050"));
    assert (equal(a, "150"));
    //assert (equalHex(a, "96"));

    a = 10;
    a += -20;
    assert (equal(a, "-10"));
    a *= 5;
    assert (equal(a, "-50"));
    b = a + 73;
    assert (equal(b, "23"));
    b = a * -7;
    assert (equal(b, "350"));
    assert (equal(a, "-50"));
   // assert (equalHex(a, "-32"));

    a = "12345678901234567890";
    a += "-99999999999999999999";
    assert (equal(a, "-87654321098765432109"));
    a *= "54321987654321987654";
    assert (equal(a, "-4761556948575111126880627366067073182286"));
    a *= 0;
    assert (equal(a, "0"));
    a = 10;
    b = a + "400";
    assert (equal(b, "410"));
    b = a * "15";
    assert (equal(b, "150"));
    assert (equal(a, "10"));
   // assert (equalHex(a, "a"));

    is.clear();
    is.str(" 1234");
    assert (is >> b);
    assert (equal(b, "1234"));
    is.clear();
    is.str(" 12 34");
    assert (is >> b);
    assert (equal(b, "12"));
    is.clear();
    is.str("999z");
    assert (is >> b);
    assert (equal(b, "999"));
    is.clear();
    is.str("abcd");
    assert (!(is >> b));
    is.clear();
    is.str("- 758");
    assert (!(is >> b));
    a = 42;
    try {
        a = "-xyz";
        assert ("missing an exception" == nullptr);
    }
    catch (const std::invalid_argument &e) {
        assert (equal(a, "42"));
    }


    a = "73786976294838206464";
    assert (equal(a, "73786976294838206464"));
  //  assert (equalHex(a, "40000000000000000"));
    assert (a < "1361129467683753853853498429727072845824");
    assert (a <= "1361129467683753853853498429727072845824");
    assert (!(a > "1361129467683753853853498429727072845824"));
    assert (!(a >= "1361129467683753853853498429727072845824"));
    assert (!(a == "1361129467683753853853498429727072845824"));
    assert (a != "1361129467683753853853498429727072845824");
    assert (!(a < "73786976294838206464"));
    assert (a <= "73786976294838206464");
    assert (!(a > "73786976294838206464"));
    assert (a >= "73786976294838206464");
    assert (a == "73786976294838206464");
    assert (!(a != "73786976294838206464"));
    assert (a < "73786976294838206465");
    assert (a <= "73786976294838206465");
    assert (!(a > "73786976294838206465"));
    assert (!(a >= "73786976294838206465"));
    assert (!(a == "73786976294838206465"));
    assert (a != "73786976294838206465");
    a = "2147483648";
    assert (!(a < -2147483648));
    assert (!(a <= -2147483648));
    assert (a > -2147483648);
    assert (a >= -2147483648);
    assert (!(a == -2147483648));
    assert (a != -2147483648);
    a = "-12345678";
    assert (!(a < -87654321));
    assert (!(a <= -87654321));
    assert (a > -87654321);
    assert (a >= -87654321);
    assert (!(a == -87654321));
    assert (a != -87654321);


    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
