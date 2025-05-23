#ifndef __PROGTEST__

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <compare>
#include <algorithm>
#include <cassert>
#include <memory>
#include <iterator>
#include <functional>
#include <stdexcept>

using namespace std::literals;

class CDummy {
public:
    CDummy(char c) : m_C(c) {}

    bool operator==(const CDummy &other) const = default;

    friend std::ostream &operator<<(std::ostream &os, const CDummy &x) {
        return os << '\'' << x.m_C << '\'';
    }

private:
    char m_C;
};

#endif /* __PROGTEST__ */

 #define TEST_EXTRA_INTERFACE

template<size_t N_>
void makeCombinations(std::vector<size_t> & inputVec, std::vector<size_t> & currComb, size_t start, std::vector<std::array<size_t, N_>> & result) {
    if (currComb.size() == N_) {
        std::array<size_t, N_> arr{};
        std::copy(currComb.begin(), currComb.end(), arr.begin());
        result.push_back(arr);
        return;
    }

    for (size_t i = start; i < inputVec.size(); i++) {
        currComb.push_back(inputVec[i]);
        makeCombinations(inputVec, currComb, i + 1, result);
        currComb.pop_back();
    }
}

template<typename T_>
struct Map {
    std::vector<T_> s_sub;
    std::vector<size_t> s_positions;
    size_t s_count;
};

template<typename T_>
class CSelfMatch {
public:
    CSelfMatch(std::initializer_list<T_> init) {
        for (auto &elem: init) {
            m_sequence.push_back(elem);
        }
    }

    template<typename Container>
    CSelfMatch(const Container &container) {
        for (auto &elem: container) {
            m_sequence.push_back(elem);
        }
    }

    template<typename Iterator>
    CSelfMatch(Iterator begin, Iterator end) {
        Iterator tmp = begin;
        while (tmp != end) {
            m_sequence.push_back(*tmp);
            tmp++;
        }
    }

    size_t sequenceLen(size_t n) const {
        if (n == 1) {
            return m_sequence.size();
        } else if (n == 0) {
            throw std::invalid_argument("Nespravny vstup.\n");
        }

        size_t maxLen = 0;
        std::vector<Map<T_>> countMap;

        for (size_t i = 0; i < m_sequence.size(); i++) {
            for (size_t l = 1; i + l <= m_sequence.size(); l++) {
                std::vector<T_> subVec(m_sequence.begin() + i, m_sequence.begin() + i + l);
                auto it = std::find_if(countMap.begin(), countMap.end(), [&](const Map<T_> &item) {
                    return subVec == item.s_sub;
                });
                if (it != countMap.end()) {
                    (*it).s_count++;
                    if ((*it).s_count >= n) {
                        maxLen = (maxLen > l) ? maxLen : l;
                    }
                } else {
                    Map<T_> newMap;
                    newMap.s_sub = subVec;
                    newMap.s_count = 1;
                    countMap.push_back(newMap);
//                    if (newMap.s_count >= n) {
//                        maxLen = (maxLen > l) ? maxLen : l;
//                    }
                }
            }
        }

        return maxLen;
    }

    template<size_t N_>
    std::vector<std::array<size_t, N_>> findSequences() const {
        if (N_ == 0) {
            throw std::invalid_argument("Nespravny vstup.\n");
        } else if (N_ == 1) {
            std::vector<std::array<size_t, N_>> result;
            result.push_back({0});
            return result;
        }

        std::vector<std::array<size_t, N_>> result;

        size_t maxLen = 0;
        std::vector<Map<T_>> countMap;

        for (size_t i = 0; i < m_sequence.size(); i++) {
            for (size_t l = 1; i + l <= m_sequence.size(); l++) {
                std::vector<T_> subVec(m_sequence.begin() + i, m_sequence.begin() + i + l);
                auto it = std::find_if(countMap.begin(), countMap.end(), [&](const Map<T_> &item) {
                    return subVec == item.s_sub;
                });
                if (it != countMap.end()) {
                    (*it).s_count++;
                    (*it).s_positions.push_back(i);
                    if ((*it).s_count >= N_) {
                        maxLen = (maxLen > l) ? maxLen : l;
                    }
                } else {
                    Map<T_> newMap;
                    newMap.s_sub = subVec;
                    newMap.s_count = 1;
                    newMap.s_positions.push_back(i);
                    countMap.push_back(newMap);
//                    if (newMap.s_count >= n) {
//                        maxLen = (maxLen > l) ? maxLen : l;
//                    }
                }
                subVec.clear();
            }
        }

        for (size_t k = 0; k < countMap.size(); k++) {
            if (countMap[k].s_count >= N_ && countMap[k].s_sub.size() == maxLen) {
                std::vector<size_t> tmp;
                makeCombinations(countMap[k].s_positions, tmp, 0, result);
            }
        }

        return result;
    }

private:
    std::vector<T_> m_sequence;
};


#ifndef __PROGTEST__

template<size_t N_>
bool positionMatch(std::vector<std::array<size_t, N_>> a, std::vector<std::array<size_t, N_>> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

int main() {
    CSelfMatch<char> x0("aaaaaaaaaaa"s);
    assert (x0.sequenceLen(2) == 10);
    assert (positionMatch(x0.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0, 1}}));
    CSelfMatch<char> x1("abababababa"s);
    assert (x1.sequenceLen(2) == 9);
    assert (positionMatch(x1.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0, 2}}));
    CSelfMatch<char> x2("abababababab"s);
    assert (x2.sequenceLen(2) == 10);
    assert (positionMatch(x2.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0, 2}}));
    CSelfMatch<char> x3("aaaaaaaaaaa"s);
    assert (x3.sequenceLen(3) == 9);
    assert (positionMatch(x3.findSequences<3>(), std::vector<std::array<size_t, 3> >{{0, 1, 2}}));
    CSelfMatch<char> x4("abababababa"s);
    assert (x4.sequenceLen(3) == 7);
    assert (positionMatch(x4.findSequences<3>(), std::vector<std::array<size_t, 3> >{{0, 2, 4}}));
    CSelfMatch<char> x5("abababababab"s);
    assert (x5.sequenceLen(3) == 8);
    assert (positionMatch(x5.findSequences<3>(), std::vector<std::array<size_t, 3> >{{0, 2, 4}}));
    CSelfMatch<char> x6("abcdXabcd"s);
    assert (x6.sequenceLen(1) == 9);
    auto a = x6.findSequences<1>();
    assert (positionMatch(x6.findSequences<1>(), std::vector<std::array<size_t, 1> >{{0}}));
    CSelfMatch<char> x7("abcdXabcd"s);
    assert (x7.sequenceLen(2) == 4);
    assert (positionMatch(x7.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0, 5}}));
    CSelfMatch<char> x8("abcdXabcdeYabcdZabcd"s);
    assert (x8.sequenceLen(2) == 4);
    assert (positionMatch(x8.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0,  5},
                                                                                     {0,  11},
                                                                                     {0,  16},
                                                                                     {5,  11},
                                                                                     {5,  16},
                                                                                     {11, 16}}));
    CSelfMatch<char> x9("abcdXabcdYabcd"s);
    assert (x9.sequenceLen(3) == 4);
    assert (positionMatch(x9.findSequences<3>(), std::vector<std::array<size_t, 3> >{{0, 5, 10}}));
    CSelfMatch<char> x10("abcdefghijklmn"s);
    assert (x10.sequenceLen(2) == 0);
    assert (positionMatch(x10.findSequences<2>(), std::vector<std::array<size_t, 2> >{}));
    CSelfMatch<char> x11("abcXabcYabcZdefXdef"s);
    assert (x11.sequenceLen(2) == 3);
    assert (positionMatch(x11.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0,  4},
                                                                                      {0,  8},
                                                                                      {4,  8},
                                                                                      {12, 16}}));
    CSelfMatch<int> x12{1, 2, 3, 1, 2, 4, 1, 2};
    assert (x12.sequenceLen(2) == 2);
    assert (positionMatch(x12.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0, 3},
                                                                                      {0, 6},
                                                                                      {3, 6}}));
    assert (x12.sequenceLen(3) == 2);
    assert (positionMatch(x12.findSequences<3>(), std::vector<std::array<size_t, 3> >{{0, 3, 6}}));
    std::initializer_list<CDummy> init13{'a', 'b', 'c', 'd', 'X', 'a', 'b', 'c', 'd', 'Y', 'a', 'b', 'c', 'd'};
    CSelfMatch<CDummy> x13(init13.begin(), init13.end());
    assert (x13.sequenceLen(2) == 4);
    assert (positionMatch(x13.findSequences<2>(), std::vector<std::array<size_t, 2> >{{0, 5},
                                                                                      {0, 10},
                                                                                      {5, 10}}));
    std::initializer_list<int> init14{1, 2, 1, 1, 2, 1, 0, 0, 1, 2, 1, 0, 1, 2, 0, 1, 2, 0, 1, 1, 1, 2, 0, 2, 0, 1, 2, 1, 0};
    CSelfMatch<int> x14(init14.begin(), init14.end());
    assert (x14.sequenceLen(2) == 5);
    assert (positionMatch(x14.findSequences<2>(), std::vector<std::array<size_t, 2> >{{11, 14},
                                                                                      {7,  24}}));
    std::initializer_list<int> init15{1, 2, 1, 1, 2, 1, 0, 0, 1, 2, 1, 0, 1, 2, 0, 1, 2, 0, 1, 1, 1, 2, 0, 2, 0, 1, 2, 1, 0};
    CSelfMatch<int> x15(init15.begin(), init15.end());
    assert (x15.sequenceLen(3) == 4);
    assert (positionMatch(x15.findSequences<3>(), std::vector<std::array<size_t, 3> >{{3, 8, 25}}));
#ifdef TEST_EXTRA_INTERFACE
    CSelfMatch y0 ( "aaaaaaaaaaa"s );
    assert ( y0 . sequenceLen ( 2 ) == 10 );

    std::string s1 ( "abcd" );
    CSelfMatch y1 ( s1 . begin (), s1 . end () );
    assert ( y1 . sequenceLen ( 2 ) == 0 );

    CSelfMatch y2 ( ""s );
    y2 . push_back ( 'a', 'b', 'c', 'X' );
    y2 . push_back ( 'a' );
    y2 . push_back ( 'b', 'c' );
    assert ( y2 . sequenceLen ( 2 ) == 3 );
#endif /* TEST_EXTRA_INTERFACE */
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
