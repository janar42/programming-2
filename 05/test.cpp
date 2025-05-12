#ifndef __PROGTEST__

#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

#endif /* __PROGTEST__ */

class CPrintable {
public:
    virtual std::string print(std::string prefix, bool isLast) const = 0;

    virtual ~CPrintable() = default;

    virtual CPrintable * point() const = 0;

    friend std::ostream & operator<<(std::ostream & os, const CPrintable & elem) {
        os << elem.print("", false);
        return os;
    }
};

class CCPU : public CPrintable {
public:
    CCPU(int coreCount, int freq) : m_coreCount(coreCount), m_freq(freq) {}

    CPrintable * point() const override {
        return new CCPU(*this);
    }

    std::string print(std::string prefix, bool isLast) const override {
        std::string plusOrSlash = isLast ? "\\" : "+";
        std::string myPrefix = prefix + (isLast ? "  " : "| ");
        std::string out = prefix + plusOrSlash + "-CPU, " + std::to_string(m_coreCount) + " cores @ " + std::to_string(m_freq) + "MHz\n";
        return out;
    }

private:
    int m_coreCount;
    int m_freq;
};

class CMemory : public CPrintable {
public:
    CMemory(int size) : m_memSize(size) {}

    CPrintable * point() const override {
        return new CMemory(*this);
    }

    std::string print(std::string prefix, bool isLast) const override {
        std::string plusOrSlash = isLast ? "\\" : "+";
        std::string myPrefix = prefix + (isLast ? "  " : "| ");
        std::string out = prefix + plusOrSlash + "-Memory, " + std::to_string(m_memSize) + " MiB\n";
        return out;
    }

private:
    int m_memSize;
};

struct Partition {
    int m_parSize;
    std::string m_parName;
};

class CDisk : public CPrintable {
public:
    static const int MAGNETIC = 0;
    static const int SSD = 1;

    CDisk(int type, int size) : m_type(type), m_diskSize(size) {}

    CPrintable * point() const override {
        return new CDisk(*this);
    }

    CDisk & addPartition(int size, const std::string & name) {
        Partition part;
        part.m_parSize = size;
        part.m_parName = name;
        m_partitions.push_back(part);
        return *this;
    }

    std::string print(std::string prefix, bool isLast) const override {
        std::string out;
        std::string diskType = (m_type == SSD) ? "SSD" : "HDD";
        std::string plusOrSlash = isLast ? "\\" : "+";
        std::string myPrefix = prefix + (isLast ? "  " : "| ");

        out += prefix + plusOrSlash + "-" + diskType + ", " + std::to_string(m_diskSize) + " GiB\n";

        if (!m_partitions.empty()) {
            int i = 0;
            for (const auto & part: m_partitions) {
                std::string plusOrSlash2 = &part == &m_partitions.back() ? "\\" : "+";
                out += myPrefix + plusOrSlash2 + "-[" + std::to_string(i) + "]: " + std::to_string(part.m_parSize) + " GiB, " + part.m_parName +
                       "\n";
                i++;
            }
        }
        return out;
    }

private:
    int m_type;
    int m_diskSize;
    std::vector<Partition> m_partitions;
};

class CComputer : public CPrintable {
public:
    CComputer(std::string & name) : m_computerName(name) {}

    CComputer(std::string name) : m_computerName(std::move(name)) {}

    CComputer * point() const override {
        return new CComputer(*this);
    }

    CComputer(const CComputer & other) : m_computerName(other.m_computerName), m_addresses(other.m_addresses) {
        for(const auto & comp: other.m_components){
            CPrintable * newPtr = comp->point();
            m_components.push_back(newPtr);
        }
    }

    CComputer & operator=(const CComputer & other) {
        if (this != &other) {
            m_computerName = other.m_computerName;
            m_addresses = other.m_addresses;

            for (auto & comp: m_components) {
                delete comp;
            }
            m_components.clear();

            for (const auto & comp: other.m_components) {
                CPrintable * newPtr = comp->point();
                m_components.push_back(newPtr);
            }
        }
        return *this;
    }

    ~CComputer() {
        for (auto & comp : m_components) {
            delete comp;
        }
        m_components.clear();
    }

    std::string getName() const {
        return m_computerName;
    }

    CComputer & addComponent(const CPrintable & elem) {
        m_components.push_back(elem.point());
        return *this;
    }

    CComputer & addAddress(const std::string & add) {
        m_addresses.push_back(add);
        return *this;
    }

    std::string print(std::string prefix, bool isLast) const override {
        std::string out;
        std::string plusOrSlash = isLast ? "\\" : "+";
        std::string myPrefix = isLast ? "  " : "| ";
        if (prefix.empty()) {
            plusOrSlash = "";
            myPrefix = "";
        }
        out += plusOrSlash + prefix + "Host: " + m_computerName + "\n";

        for (const auto & add: m_addresses) {
            out += myPrefix + "+-" + add + "\n";
        }

        for (CPrintable* component: m_components) {
            out += component->print(myPrefix, component == m_components.back());
        }

        return out;
    }

private:
    std::string m_computerName;
    std::vector<std::string> m_addresses;
    std::vector<CPrintable*> m_components;
};

class CNetwork : public CPrintable {
public:
    CNetwork(std::string & name) : m_networkName(name) {}

    CNetwork(std::string name) : m_networkName(std::move(name)) {}

    CPrintable * point() const override {
        return new CNetwork(*this);
    }

    CNetwork(const CNetwork &other): m_networkName(other.m_networkName) {
        for (const auto & comp : other.m_computers) {
            CComputer * newPtr = comp->point();
            m_computers.push_back(newPtr);
        }
    }

    CNetwork & operator=(const CNetwork & other) {
        if (this != &other) {
            m_networkName = other.m_networkName;

            for (auto & comp: m_computers) {
                delete comp;
            }
            m_computers.clear();

            for (const auto & comp: other.m_computers) {
                CComputer * newPtr = comp->point();
                m_computers.push_back(newPtr);
            }
        }
        return *this;
    }

    ~CNetwork() {
        for (auto comp : m_computers) {
            delete comp;
        }
        m_computers.clear();
    }

    std::string print(std::string prefix, bool isLast) const override {
        std::string out;
        out += prefix + "Network: " + m_networkName + "\n";

        for (const auto & comp: m_computers) {
            out += comp->print("-", &comp == & m_computers.back());
        }
        return out;
    }

    CNetwork & addComputer(const CComputer &comp) {
        m_computers.push_back(comp.point());
        return *this;
    }

    CComputer * findComputer(const std::string &name) const {
        auto it = std::find_if(m_computers.begin(), m_computers.end(), [&](const CComputer * comp) {
            return comp->getName() == name;
        });
        if (it != m_computers.end()) {
            return *it;
        } else {
            return nullptr;
        }
    }

private:
    std::string m_networkName;
    std::vector<CComputer*> m_computers;
};

#ifndef __PROGTEST__

template<typename T_>
std::string toString(const T_ &x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();
}

int main() {
    CNetwork n("FIT network");
    n.addComputer(
            CComputer("progtest.fit.cvut.cz").
                    addAddress("147.32.232.142").
                    addComponent(CCPU(8, 2400)).
                    addComponent(CCPU(8, 1200)).
                    addComponent(CDisk(CDisk::MAGNETIC, 1500).
                    addPartition(50, "/").
                    addPartition(5, "/boot").
                    addPartition(1000, "/var")).
                    addComponent(CDisk(CDisk::SSD, 60).
                    addPartition(60, "/data")).
                    addComponent(CMemory(2000)).
                    addComponent(CMemory(2000))).
            addComputer(
            CComputer("courses.fit.cvut.cz").
                    addAddress("147.32.232.213").
                    addComponent(CCPU(4, 1600)).
                    addComponent(CMemory(4000)).
                    addComponent(CDisk(CDisk::MAGNETIC, 2000).
                    addPartition(100, "/").
                    addPartition(1900, "/data"))).
            addComputer(
            CComputer("imap.fit.cvut.cz").
                    addAddress("147.32.232.238").
                    addComponent(CCPU(4, 2500)).
                    addAddress("2001:718:2:2901::238").
                    addComponent(CMemory(8000)));
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: courses.fit.cvut.cz\n"
    "| +-147.32.232.213\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "\\-Host: imap.fit.cvut.cz\n"
    "  +-147.32.232.238\n"
    "  +-2001:718:2:2901::238\n"
    "  +-CPU, 4 cores @ 2500MHz\n"
    "  \\-Memory, 8000 MiB\n" );
  CNetwork x = n;
  auto c = x . findComputer ( "imap.fit.cvut.cz" );
  assert ( toString ( *c ) ==
    "Host: imap.fit.cvut.cz\n"
    "+-147.32.232.238\n"
    "+-2001:718:2:2901::238\n"
    "+-CPU, 4 cores @ 2500MHz\n"
    "\\-Memory, 8000 MiB\n" );
  c -> addComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
         addPartition ( 100, "system" ) .
         addPartition ( 200, "WWW" ) .
         addPartition ( 700, "mail" ) );
  assert ( toString ( x ) ==
                   "Network: FIT network\n"
                   "+-Host: progtest.fit.cvut.cz\n"
                   "| +-147.32.232.142\n"
                   "| +-CPU, 8 cores @ 2400MHz\n"
                   "| +-CPU, 8 cores @ 1200MHz\n"
                   "| +-HDD, 1500 GiB\n"
                   "| | +-[0]: 50 GiB, /\n"
                   "| | +-[1]: 5 GiB, /boot\n"
                   "| | \\-[2]: 1000 GiB, /var\n"
                   "| +-SSD, 60 GiB\n"
                   "| | \\-[0]: 60 GiB, /data\n"
                   "| +-Memory, 2000 MiB\n"
                   "| \\-Memory, 2000 MiB\n"
                   "+-Host: courses.fit.cvut.cz\n"
                   "| +-147.32.232.213\n"
                   "| +-CPU, 4 cores @ 1600MHz\n"
                   "| +-Memory, 4000 MiB\n"
                   "| \\-HDD, 2000 GiB\n"
                   "|   +-[0]: 100 GiB, /\n"
                   "|   \\-[1]: 1900 GiB, /data\n"
                   "\\-Host: imap.fit.cvut.cz\n"
                   "  +-147.32.232.238\n"
                   "  +-2001:718:2:2901::238\n"
                   "  +-CPU, 4 cores @ 2500MHz\n"
                   "  +-Memory, 8000 MiB\n"
                   "  \\-HDD, 1000 GiB\n"
                   "    +-[0]: 100 GiB, system\n"
                   "    +-[1]: 200 GiB, WWW\n"
                   "    \\-[2]: 700 GiB, mail\n" );
  auto asd = toString(n);
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: courses.fit.cvut.cz\n"
    "| +-147.32.232.213\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "\\-Host: imap.fit.cvut.cz\n"
    "  +-147.32.232.238\n"
    "  +-2001:718:2:2901::238\n"
    "  +-CPU, 4 cores @ 2500MHz\n"
    "  \\-Memory, 8000 MiB\n" );
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
