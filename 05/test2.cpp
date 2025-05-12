#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
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
    virtual std::string print(std::string prefix, bool isLast, bool isFirst) const = 0;

    virtual ~CPrintable() = default;

    virtual CPrintable * copy(const std::map<std::string, std::string> & remap) const = 0;

    friend std::ostream & operator<<(std::ostream & os, const CPrintable & elem) {
        os << elem.print("", false, true);
        return os;
    }
};

class CCPU : public CPrintable {
public:
    CCPU(int coreCount, int freq) : m_coreCount(coreCount), m_freq(freq) {}

    CPrintable * copy(const std::map<std::string, std::string> & remap) const override {
        return new CCPU(*this);
    }

    std::string print(std::string prefix, bool isLast, bool isFirst) const override {
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

    CPrintable * copy(const std::map<std::string, std::string> & remap) const override {
        return new CMemory(*this);
    }

    std::string print(std::string prefix, bool isLast, bool isFirst) const override {
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

    CPrintable * copy(const std::map<std::string, std::string> & remap) const override {
        return new CDisk(*this);
    }

    CDisk & addPartition(int size, const std::string & name) {
        Partition part;
        part.m_parSize = size;
        part.m_parName = name;
        m_partitions.push_back(part);
        return *this;
    }

    std::string print(std::string prefix, bool isLast, bool isFirst) const override {
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

class CComputer;
class CNetwork: public CPrintable {
public:
    CNetwork(const std::string & name);

    CPrintable * copy(const std::map<std::string, std::string> & remap) const override;

    CNetwork(const CNetwork &other);

    CNetwork & operator=(const CNetwork & other);

    ~CNetwork();

    std::string getName() const;

    std::string print(std::string prefix, bool isLast, bool isFirst) const override;

    CNetwork & addComputer(const CComputer & comp);

    CComputer * findComputer(const std::string & name) const;

    CNetwork * findNetwork(const std::string & name) const;

private:
    std::string m_networkName;
    std::vector<CComputer*> m_computers;
};

class CComputer : public CPrintable {
public:
    CComputer(const std::string & name);

    CComputer * copy(const std::map<std::string, std::string> & remap) const override;

    CComputer(const CComputer & other);

    CComputer & operator=(const CComputer & other);

    ~CComputer();

    std::string getName() const;

    CComputer & addComponent(const CPrintable & elem);

    CComputer & addAddress(const std::string & add);

    CComputer * findComputer(const std::string & name) const;

    CNetwork * findNetwork(const std::string & name) const;

    const std::vector<CPrintable *> & components() const;

    CComputer duplicate(const std::map<std::string, std::string> & remap) const;


    std::string print(std::string prefix, bool isLast, bool isFirst) const override;

private:
    std::string m_computerName;
    std::vector<std::string> m_addresses;
    std::vector<CPrintable*> m_components;
};


CNetwork::CNetwork(const std::string & name) : m_networkName(name) {}

CNetwork::CPrintable * CNetwork::copy(const std::map<std::string, std::string> & remap) const {
    std::string newName = m_networkName;
    auto it = remap.find(m_networkName);
    if (it != remap.end()) {
        newName.assign(it->second);
    }

    auto cn = new CNetwork(newName);
    for (CComputer * c: this->m_computers) {
        cn->m_computers.push_back(c->copy(remap));
    }
    return cn;
}


CComputer::CComputer(const std::string & name) : m_computerName(name) {}

CComputer * CComputer::copy(const std::map<std::string, std::string> & remap) const {
    std::string newName = m_computerName;
    auto it = remap.find(m_computerName);
    if (it != remap.end()) {
        newName.assign(it->second);
    }

    auto ret = new CComputer(newName);
    for(auto component: m_components) {
        ret->m_components.push_back(component->copy(remap));
    }

    for(auto ad: m_addresses) {
        ret->m_addresses.push_back(ad);
    }

    return ret;
}

CComputer::CComputer(const CComputer & other) : m_computerName(other.m_computerName), m_addresses(other.m_addresses) {
    for(const auto & comp: other.m_components){
        CPrintable * newPtr = comp->copy({});
        m_components.push_back(newPtr);
    }
}

CComputer & CComputer::operator=(const CComputer & other) {
    if (this != &other) {
        m_computerName = other.m_computerName;
        m_addresses = other.m_addresses;

        for (auto & comp: m_components) {
            delete comp;
        }
        m_components.clear();

        for (const auto & comp: other.m_components) {
            CPrintable * newPtr = comp->copy({});
            m_components.push_back(newPtr);
        }
    }
    return *this;
}

CComputer::~CComputer() {
    for (auto & comp : m_components) {
        delete comp;
    }
    m_components.clear();
}

std::string CComputer::getName() const {
    return m_computerName;
}

CComputer & CComputer::addComponent(const CPrintable & elem) {
    m_components.push_back(elem.copy({}));
    return *this;
}

CComputer & CComputer::addAddress(const std::string & add) {
    m_addresses.push_back(add);
    return *this;
}

CComputer * CComputer::findComputer(const std::string & name) const {
    if (m_computerName == name) {
        return this->copy({});
    }

    for (const auto & comp : m_components) {
        auto it = dynamic_cast<CNetwork*>(comp);
        if (it) {
            CComputer * found = it->findComputer(name);
            if (found) {
                return found;
            }
        }
    }
    return nullptr;
}

CNetwork * CComputer::findNetwork(const std::string & name) const {
    auto it = std::find_if(m_components.begin(), m_components.end(), [&](const CPrintable * comp) {
        if (auto it = dynamic_cast<const CNetwork*>(comp)) {
            return it->getName() == name;
        }
        return false;
    });
    if (it != m_components.end()) {
        return (CNetwork *) *it;
    } else {
        return nullptr;
    }
}


const std::vector<CPrintable *> & CComputer::components() const {
    return this->m_components;
}

CComputer CComputer::duplicate(const std::map<std::string, std::string> & remap) const {
    std::string newName = m_computerName;
    auto it = remap.find(m_computerName);
    if (it != remap.end()) {
        newName.assign(it->second);
    }

    auto ret = CComputer(std::move(newName));
    for(auto a: m_addresses) {
        ret.m_addresses.push_back(a);
    }
    for (auto component: m_components) {
        ret.m_components.push_back(component->copy(remap));
    }
    return ret;
}

std::string CComputer::print(std::string prefix, bool isLast, bool isFirst) const  {
    std::string out;
    std::string plusOrSlash = isLast ? "\\" : "+";
    std::string nextPrefix = isLast ? "  " : "| ";

    if (isFirst) {
        out = "Host: " + m_computerName + "\n";
        nextPrefix = "";
    } else {
        out += prefix + plusOrSlash + "-Host: " + m_computerName + "\n";
    }

    for (const auto & add: m_addresses) {
        out += prefix + nextPrefix + "+-" + add + "\n";
    }

    for (CPrintable* component: m_components) {
        out += component->print(prefix + nextPrefix, component == m_components.back(), false);
    }

    return out;
}


CNetwork::CNetwork(const CNetwork &other): m_networkName(other.m_networkName) {
    for (const auto & comp : other.m_computers) {
        CComputer * newPtr = comp->copy({});
        m_computers.push_back(newPtr);
    }
}

CNetwork &CNetwork::operator=(const CNetwork &other) {
    if (this != &other) {
        m_networkName = other.m_networkName;

        for (auto & comp: m_computers) {
            delete comp;
        }
        m_computers.clear();

        for (const auto & comp: other.m_computers) {
            CComputer * newPtr = comp->copy({});
            m_computers.push_back(newPtr);
        }
    }
    return *this;
}

CNetwork::~CNetwork() {
    for (auto comp : m_computers) {
        delete comp;
    }
    m_computers.clear();
}

std::string CNetwork::getName() const {
    return m_networkName;
}

std::string CNetwork::print(std::string prefix, bool isLast, bool isFirst) const {
    std::string out;
    std::string plusOrSlash = isLast ? "\\" : "+";
    std::string nextPrefix = isLast ? "  " : "| ";

    if (isFirst) {
        out = "Network: " + m_networkName + "\n";
        nextPrefix = "";
    } else {
        out += prefix + plusOrSlash + "-Network: " + m_networkName + "\n";
    }

    for (const auto & comp: m_computers) {
        out += comp->print(prefix + nextPrefix, &comp == &m_computers.back(), false);
    }
    return out;
}

CNetwork & CNetwork::addComputer(const CComputer & comp) {
    m_computers.push_back(comp.copy({}));
    return *this;
}

CComputer * CNetwork::findComputer(const std::string &name) const {
    for (const auto & computer : m_computers) {
        if (computer->getName() == name) {
            return computer;
        } else {
            for(auto component: computer->components()) {
                if (auto network = dynamic_cast<CNetwork*>(component)) {
                    if (auto result = network->findComputer(name)) {
                        return result;
                    }
                }
            }
        }
    }

    return nullptr;
}

CNetwork * CNetwork::findNetwork(const std::string &name) const {
    if (m_networkName == name) {
        return const_cast<CNetwork *>(this);
    }

    for (const auto & comp : m_computers) {
        auto found = comp->findNetwork(name);
        if (found) {
            return found;
        }
    }

    return nullptr;
}


#ifndef __PROGTEST__
template<typename T_>
std::string toString ( const T_ & x )
{
  std::ostringstream oss;
  oss << x;
  return oss . str ();
}

int main ()
{
  CNetwork n ( "FIT network" );
  n . addComputer (
      CComputer ( "progtest.fit.cvut.cz" ) .
        addAddress ( "147.32.232.142" ) .
        addComponent ( CCPU ( 8, 2400 ) ) .
        addComponent ( CCPU ( 8, 1200 ) ) .
        addComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
          addPartition ( 50, "/" ) .
          addPartition ( 5, "/boot" ).
          addPartition ( 1000, "/var" ) ) .
        addComponent ( CDisk ( CDisk::SSD, 60 ) .
          addPartition ( 60, "/data" )  ) .
        addComponent ( CMemory ( 2000 ) ).
        addComponent ( CMemory ( 2000 ) ) ) .
    addComputer (
      CComputer ( "courses.fit.cvut.cz" ) .
        addAddress ( "147.32.232.213" ) .
        addComponent ( CCPU ( 4, 1600 ) ) .
        addComponent ( CMemory ( 4000 ) ).
        addComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
          addPartition ( 100, "/" )   .
          addPartition ( 1900, "/data" ) ) ) .
    addComputer (
      CComputer ( "imap.fit.cvut.cz" ) .
        addAddress ( "147.32.232.238" ) .
        addComponent ( CCPU ( 4, 2500 ) ) .
        addAddress ( "2001:718:2:2901::238" ) .
        addComponent ( CMemory ( 8000 ) ) );

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
  n . addComputer ( CComputer ( "vm01.fit.cvut.cz" ) .
    addAddress ( "147.32.232.232" ) .
      addComponent ( CCPU ( 32, 4000 ) ) .
      addComponent ( CMemory ( 32768 ) ) .
      addComponent ( CDisk ( CDisk::MAGNETIC, 3000 ) .
        addPartition ( 500, "SYSTEM" ) .
        addPartition ( 2000, "DATA" ) .
        addPartition ( 500, "BACKUP" ) ) .
      addComponent ( CNetwork ( "dummy00" ) .
        addComputer ( CComputer ( "testing.fit.cvut.cz" ) .
          addAddress ( "192.168.1.1" ) .
          addComponent ( CCPU ( 1, 300 ) ) .
          addComponent ( CMemory ( 256 ) ) ) ) .
      addComponent ( CMemory ( 16384 ) ) .
      addComponent ( CNetwork ( "vnet00" ) ) );
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
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "\\-Host: vm01.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n" );
  auto vmnet = n . findNetwork ( "vnet00" );
  vmnet -> addComputer ( CComputer ( "monitor1.fit.cvut.cz" ) .
    addAddress ( "147.32.232.254" ) .
    addComponent ( CCPU ( 2, 1500 ) ) .
    addComponent ( CMemory ( 4096 ) ) .
    addComponent ( CDisk ( CDisk::MAGNETIC, 750 ) .
      addPartition ( 100, "root" ) .
      addPartition ( 600, "log" ) ) );
  assert ( toString ( *vmnet ) ==
    "Network: vnet00\n"
    "\\-Host: monitor1.fit.cvut.cz\n"
    "  +-147.32.232.254\n"
    "  +-CPU, 2 cores @ 1500MHz\n"
    "  +-Memory, 4096 MiB\n"
    "  \\-HDD, 750 GiB\n"
    "    +-[0]: 100 GiB, root\n"
    "    \\-[1]: 600 GiB, log\n" );
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
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "\\-Host: vm01.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n"
    "    \\-Host: monitor1.fit.cvut.cz\n"
    "      +-147.32.232.254\n"
    "      +-CPU, 2 cores @ 1500MHz\n"
    "      +-Memory, 4096 MiB\n"
    "      \\-HDD, 750 GiB\n"
    "        +-[0]: 100 GiB, root\n"
    "        \\-[1]: 600 GiB, log\n" );

  vmnet -> addComputer (
          n . findComputer ( "monitor1.fit.cvut.cz" ) -> duplicate ( { std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor2.fit.cvut.cz") } )
  );
  assert ( toString ( *vmnet ) ==
    "Network: vnet00\n"
    "+-Host: monitor1.fit.cvut.cz\n"
    "| +-147.32.232.254\n"
    "| +-CPU, 2 cores @ 1500MHz\n"
    "| +-Memory, 4096 MiB\n"
    "| \\-HDD, 750 GiB\n"
    "|   +-[0]: 100 GiB, root\n"
    "|   \\-[1]: 600 GiB, log\n"
    "\\-Host: monitor2.fit.cvut.cz\n"
    "  +-147.32.232.254\n"
    "  +-CPU, 2 cores @ 1500MHz\n"
    "  +-Memory, 4096 MiB\n"
    "  \\-HDD, 750 GiB\n"
    "    +-[0]: 100 GiB, root\n"
    "    \\-[1]: 600 GiB, log\n" );
  n . addComputer ( n . findComputer ( "vm01.fit.cvut.cz" )
        -> duplicate (
           {
             std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor3.fit.cvut.cz"),
             std::pair<std::string,std::string>("monitor2.fit.cvut.cz","monitor4.fit.cvut.cz"),
             std::pair<std::string,std::string>("vm01.fit.cvut.cz","vm02.fit.cvut.cz")
           } ) );
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
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "+-Host: vm01.fit.cvut.cz\n"
    "| +-147.32.232.232\n"
    "| +-CPU, 32 cores @ 4000MHz\n"
    "| +-Memory, 32768 MiB\n"
    "| +-HDD, 3000 GiB\n"
    "| | +-[0]: 500 GiB, SYSTEM\n"
    "| | +-[1]: 2000 GiB, DATA\n"
    "| | \\-[2]: 500 GiB, BACKUP\n"
    "| +-Network: dummy00\n"
    "| | \\-Host: testing.fit.cvut.cz\n"
    "| |   +-192.168.1.1\n"
    "| |   +-CPU, 1 cores @ 300MHz\n"
    "| |   \\-Memory, 256 MiB\n"
    "| +-Memory, 16384 MiB\n"
    "| \\-Network: vnet00\n"
    "|   +-Host: monitor1.fit.cvut.cz\n"
    "|   | +-147.32.232.254\n"
    "|   | +-CPU, 2 cores @ 1500MHz\n"
    "|   | +-Memory, 4096 MiB\n"
    "|   | \\-HDD, 750 GiB\n"
    "|   |   +-[0]: 100 GiB, root\n"
    "|   |   \\-[1]: 600 GiB, log\n"
    "|   \\-Host: monitor2.fit.cvut.cz\n"
    "|     +-147.32.232.254\n"
    "|     +-CPU, 2 cores @ 1500MHz\n"
    "|     +-Memory, 4096 MiB\n"
    "|     \\-HDD, 750 GiB\n"
    "|       +-[0]: 100 GiB, root\n"
    "|       \\-[1]: 600 GiB, log\n"
    "\\-Host: vm02.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n"
    "    +-Host: monitor3.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    \\-Host: monitor4.fit.cvut.cz\n"
    "      +-147.32.232.254\n"
    "      +-CPU, 2 cores @ 1500MHz\n"
    "      +-Memory, 4096 MiB\n"
    "      \\-HDD, 750 GiB\n"
    "        +-[0]: 100 GiB, root\n"
    "        \\-[1]: 600 GiB, log\n" );
  vmnet -> addComputer ( n . findComputer ( "vm02.fit.cvut.cz" )
           -> duplicate (
              {
                std::pair<std::string,std::string>("monitor3.fit.cvut.cz","monitor5.fit.cvut.cz"),
                std::pair<std::string,std::string>("monitor4.fit.cvut.cz","monitor6.fit.cvut.cz"),
                std::pair<std::string,std::string>("vm02.fit.cvut.cz","vm03.fit.cvut.cz"),
                std::pair<std::string,std::string>("vnet00","vnet01")
              } ) );
  assert ( toString ( *vmnet ) ==
    "Network: vnet00\n"
    "+-Host: monitor1.fit.cvut.cz\n"
    "| +-147.32.232.254\n"
    "| +-CPU, 2 cores @ 1500MHz\n"
    "| +-Memory, 4096 MiB\n"
    "| \\-HDD, 750 GiB\n"
    "|   +-[0]: 100 GiB, root\n"
    "|   \\-[1]: 600 GiB, log\n"
    "+-Host: monitor2.fit.cvut.cz\n"
    "| +-147.32.232.254\n"
    "| +-CPU, 2 cores @ 1500MHz\n"
    "| +-Memory, 4096 MiB\n"
    "| \\-HDD, 750 GiB\n"
    "|   +-[0]: 100 GiB, root\n"
    "|   \\-[1]: 600 GiB, log\n"
    "\\-Host: vm03.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet01\n"
    "    +-Host: monitor5.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    \\-Host: monitor6.fit.cvut.cz\n"
    "      +-147.32.232.254\n"
    "      +-CPU, 2 cores @ 1500MHz\n"
    "      +-Memory, 4096 MiB\n"
    "      \\-HDD, 750 GiB\n"
    "        +-[0]: 100 GiB, root\n"
    "        \\-[1]: 600 GiB, log\n" );
  n . findComputer ( "vm02.fit.cvut.cz" )
      -> findNetwork ( "vnet00" )
      -> addComputer ( n . findComputer ( "vm01.fit.cvut.cz" )
           -> duplicate (
              {
                std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor11.fit.cvut.cz"),
                std::pair<std::string,std::string>("monitor2.fit.cvut.cz","monitor12.fit.cvut.cz"),
                std::pair<std::string,std::string>("vm01.fit.cvut.cz","vm11.fit.cvut.cz"),
                std::pair<std::string,std::string>("vm03.fit.cvut.cz","vm13.fit.cvut.cz"),
                std::pair<std::string,std::string>("vnet00","vnet10"),
                std::pair<std::string,std::string>("vnet01","vnet11")
              } ) );
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
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "+-Host: vm01.fit.cvut.cz\n"
    "| +-147.32.232.232\n"
    "| +-CPU, 32 cores @ 4000MHz\n"
    "| +-Memory, 32768 MiB\n"
    "| +-HDD, 3000 GiB\n"
    "| | +-[0]: 500 GiB, SYSTEM\n"
    "| | +-[1]: 2000 GiB, DATA\n"
    "| | \\-[2]: 500 GiB, BACKUP\n"
    "| +-Network: dummy00\n"
    "| | \\-Host: testing.fit.cvut.cz\n"
    "| |   +-192.168.1.1\n"
    "| |   +-CPU, 1 cores @ 300MHz\n"
    "| |   \\-Memory, 256 MiB\n"
    "| +-Memory, 16384 MiB\n"
    "| \\-Network: vnet00\n"
    "|   +-Host: monitor1.fit.cvut.cz\n"
    "|   | +-147.32.232.254\n"
    "|   | +-CPU, 2 cores @ 1500MHz\n"
    "|   | +-Memory, 4096 MiB\n"
    "|   | \\-HDD, 750 GiB\n"
    "|   |   +-[0]: 100 GiB, root\n"
    "|   |   \\-[1]: 600 GiB, log\n"
    "|   +-Host: monitor2.fit.cvut.cz\n"
    "|   | +-147.32.232.254\n"
    "|   | +-CPU, 2 cores @ 1500MHz\n"
    "|   | +-Memory, 4096 MiB\n"
    "|   | \\-HDD, 750 GiB\n"
    "|   |   +-[0]: 100 GiB, root\n"
    "|   |   \\-[1]: 600 GiB, log\n"
    "|   \\-Host: vm03.fit.cvut.cz\n"
    "|     +-147.32.232.232\n"
    "|     +-CPU, 32 cores @ 4000MHz\n"
    "|     +-Memory, 32768 MiB\n"
    "|     +-HDD, 3000 GiB\n"
    "|     | +-[0]: 500 GiB, SYSTEM\n"
    "|     | +-[1]: 2000 GiB, DATA\n"
    "|     | \\-[2]: 500 GiB, BACKUP\n"
    "|     +-Network: dummy00\n"
    "|     | \\-Host: testing.fit.cvut.cz\n"
    "|     |   +-192.168.1.1\n"
    "|     |   +-CPU, 1 cores @ 300MHz\n"
    "|     |   \\-Memory, 256 MiB\n"
    "|     +-Memory, 16384 MiB\n"
    "|     \\-Network: vnet01\n"
    "|       +-Host: monitor5.fit.cvut.cz\n"
    "|       | +-147.32.232.254\n"
    "|       | +-CPU, 2 cores @ 1500MHz\n"
    "|       | +-Memory, 4096 MiB\n"
    "|       | \\-HDD, 750 GiB\n"
    "|       |   +-[0]: 100 GiB, root\n"
    "|       |   \\-[1]: 600 GiB, log\n"
    "|       \\-Host: monitor6.fit.cvut.cz\n"
    "|         +-147.32.232.254\n"
    "|         +-CPU, 2 cores @ 1500MHz\n"
    "|         +-Memory, 4096 MiB\n"
    "|         \\-HDD, 750 GiB\n"
    "|           +-[0]: 100 GiB, root\n"
    "|           \\-[1]: 600 GiB, log\n"
    "\\-Host: vm02.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n"
    "    +-Host: monitor3.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    +-Host: monitor4.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    \\-Host: vm11.fit.cvut.cz\n"
    "      +-147.32.232.232\n"
    "      +-CPU, 32 cores @ 4000MHz\n"
    "      +-Memory, 32768 MiB\n"
    "      +-HDD, 3000 GiB\n"
    "      | +-[0]: 500 GiB, SYSTEM\n"
    "      | +-[1]: 2000 GiB, DATA\n"
    "      | \\-[2]: 500 GiB, BACKUP\n"
    "      +-Network: dummy00\n"
    "      | \\-Host: testing.fit.cvut.cz\n"
    "      |   +-192.168.1.1\n"
    "      |   +-CPU, 1 cores @ 300MHz\n"
    "      |   \\-Memory, 256 MiB\n"
    "      +-Memory, 16384 MiB\n"
    "      \\-Network: vnet10\n"
    "        +-Host: monitor11.fit.cvut.cz\n"
    "        | +-147.32.232.254\n"
    "        | +-CPU, 2 cores @ 1500MHz\n"
    "        | +-Memory, 4096 MiB\n"
    "        | \\-HDD, 750 GiB\n"
    "        |   +-[0]: 100 GiB, root\n"
    "        |   \\-[1]: 600 GiB, log\n"
    "        +-Host: monitor12.fit.cvut.cz\n"
    "        | +-147.32.232.254\n"
    "        | +-CPU, 2 cores @ 1500MHz\n"
    "        | +-Memory, 4096 MiB\n"
    "        | \\-HDD, 750 GiB\n"
    "        |   +-[0]: 100 GiB, root\n"
    "        |   \\-[1]: 600 GiB, log\n"
    "        \\-Host: vm13.fit.cvut.cz\n"
    "          +-147.32.232.232\n"
    "          +-CPU, 32 cores @ 4000MHz\n"
    "          +-Memory, 32768 MiB\n"
    "          +-HDD, 3000 GiB\n"
    "          | +-[0]: 500 GiB, SYSTEM\n"
    "          | +-[1]: 2000 GiB, DATA\n"
    "          | \\-[2]: 500 GiB, BACKUP\n"
    "          +-Network: dummy00\n"
    "          | \\-Host: testing.fit.cvut.cz\n"
    "          |   +-192.168.1.1\n"
    "          |   +-CPU, 1 cores @ 300MHz\n"
    "          |   \\-Memory, 256 MiB\n"
    "          +-Memory, 16384 MiB\n"
    "          \\-Network: vnet11\n"
    "            +-Host: monitor5.fit.cvut.cz\n"
    "            | +-147.32.232.254\n"
    "            | +-CPU, 2 cores @ 1500MHz\n"
    "            | +-Memory, 4096 MiB\n"
    "            | \\-HDD, 750 GiB\n"
    "            |   +-[0]: 100 GiB, root\n"
    "            |   \\-[1]: 600 GiB, log\n"
    "            \\-Host: monitor6.fit.cvut.cz\n"
    "              +-147.32.232.254\n"
    "              +-CPU, 2 cores @ 1500MHz\n"
    "              +-Memory, 4096 MiB\n"
    "              \\-HDD, 750 GiB\n"
    "                +-[0]: 100 GiB, root\n"
    "                \\-[1]: 600 GiB, log\n" );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
