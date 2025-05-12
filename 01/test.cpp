#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

#endif /* __PROGTEST__ */

std::string lower(const std::string &inp) {
    std::string ret = inp;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    return ret;
}

bool stringCaseCmp(const std::string &str1, const std::string &str2) {
    std::string str1Lower = str1;
    std::string str2Lower = str2;
    std::transform(str1Lower.begin(), str1Lower.end(), str1Lower.begin(), ::tolower);
    std::transform(str2Lower.begin(), str2Lower.end(), str2Lower.begin(), ::tolower);

    return str1Lower == str2Lower;
}

struct Property {
    std::string m_region;
    unsigned int m_id;
    std::string m_city;
    std::string m_addr;
    std::string m_owner;
    std::string m_iowner;
    size_t m_ownershipOrder;
};

struct Owner {
    std::string o_name;
    size_t o_count;
};

class CIterator;

class CLandRegister {
    friend class CIterator;

public:

    bool add(const std::string &city, const std::string &addr, const std::string &region, unsigned int id) {
        if (find(city, addr) != nullptr || find(region, id) != nullptr) {
            return false;
        }
        Property newProperty;
        newProperty.m_city = city;
        newProperty.m_addr = addr;
        newProperty.m_region = region;
        newProperty.m_id = id;
        newProperty.m_owner = "";
        newProperty.m_iowner = "";
        newProperty.m_ownershipOrder = count("");

        auto newPropPtr = std::make_shared<Property>(newProperty);
        insertCityAddress(newPropPtr);
        insertRegionID(newPropPtr);
        insertOwner(newProperty.m_iowner);

        return true;
    }

    bool del(const std::string &city, const std::string &addr) {
        auto found = find(city, addr);
        if (found == nullptr) {
            return false;
        }

        m_propertiesCityAddr.erase(std::remove_if(m_propertiesCityAddr.begin(), m_propertiesCityAddr.end(), [&](const std::shared_ptr<Property> &prop) {
            return (*prop).m_city == city && (*prop).m_addr == addr;
        }), m_propertiesCityAddr.end());

        m_propertiesRegionID.erase(std::remove_if(m_propertiesRegionID.begin(), m_propertiesRegionID.end(), [&](const std::shared_ptr<Property> &prop) {
            return (*prop).m_city == city && (*prop).m_addr == addr;
        }), m_propertiesRegionID.end());

        delOwner(found->m_iowner);

        return true;
    }

    bool del(const std::string &region, unsigned int id) {
        auto found = find(region, id);
        if (found == nullptr) {
            return false;
        }

        m_propertiesCityAddr.erase(std::remove_if(m_propertiesCityAddr.begin(), m_propertiesCityAddr.end(), [&](const std::shared_ptr<Property> &prop) {
            return (*prop).m_region == region && (*prop).m_id == id;
        }), m_propertiesCityAddr.end());

        m_propertiesRegionID.erase(std::remove_if(m_propertiesRegionID.begin(), m_propertiesRegionID.end(), [&](const std::shared_ptr<Property> &prop) {
            return (*prop).m_region == region && (*prop).m_id == id;
        }), m_propertiesRegionID.end());

        delOwner(found->m_iowner);

        return true;
    }

    bool getOwner(const std::string &city, const std::string &addr, std::string &owner) const {
        std::shared_ptr<Property> prop = find(city, addr);
        if (prop != nullptr) {
            owner = prop->m_owner;
            return true;
        }
        return false;
    }

    bool getOwner(const std::string &region, unsigned int id, std::string &owner) const {
        std::shared_ptr<Property> prop = find(region, id);
        if (prop != nullptr) {
            owner = prop->m_owner;
            return true;
        }
        return false;
    }

    size_t count(const std::string &owner) const {
        std::shared_ptr<Owner> found = find(lower(owner));
        if (found == nullptr) {
            return 0;
        }
        return found->o_count;
    }

    bool newOwner(const std::string &city, const std::string &addr, const std::string &owner) {
        std::shared_ptr<Property> prop = find(city, addr);
        if (prop == nullptr || stringCaseCmp(prop->m_owner, owner)) {
            return false;
        }
        std::string oldOwner = prop->m_iowner;
        delOwner(oldOwner);
        insertOwner(lower(owner));
        prop->m_owner = owner;
        prop->m_iowner = lower(owner);
        prop->m_ownershipOrder = this->count(lower(owner));
        return true;
    }

    bool newOwner(const std::string &region, unsigned int id, const std::string &owner) {
        std::shared_ptr<Property> prop = find(region, id);
        if (prop == nullptr || stringCaseCmp(prop->m_owner, owner)) {
            return false;
        }
        std::string oldOwner = prop->m_iowner;
        delOwner(oldOwner);
        insertOwner(lower(owner));
        prop->m_owner = owner;
        prop->m_iowner = lower(owner);
        prop->m_ownershipOrder = this->count(lower(owner));
        return true;
    }

    CIterator listByAddr() const;

    CIterator listByOwner(const std::string &owner) const;

private:
    std::vector<std::shared_ptr<Property>> m_propertiesCityAddr;
    std::vector<std::shared_ptr<Property>> m_propertiesRegionID;
    std::vector<std::shared_ptr<Owner>> m_owners;

    void insertCityAddress(const std::shared_ptr<Property> &prop) {
        auto it = std::lower_bound(m_propertiesCityAddr.begin(), m_propertiesCityAddr.end(), prop, [](const std::shared_ptr<Property> &a, const std::shared_ptr<Property>& b) {
            return a->m_city != b->m_city
                   ? a->m_city < b->m_city
                   : a->m_addr < b->m_addr;
        });
        m_propertiesCityAddr.insert(it, prop);
    }

    void insertRegionID(const std::shared_ptr<Property> &prop) {
        auto it = std::lower_bound(m_propertiesRegionID.begin(), m_propertiesRegionID.end(), prop, [](const std::shared_ptr<Property> &a, const std::shared_ptr<Property> &b) {
            if (a->m_region != b->m_region) return a->m_region < b->m_region;
            return a->m_id < b->m_id;
        });
        m_propertiesRegionID.insert(it, prop);
    }

    void insertOwner(const std::string &name) {
        Owner newOwner;
        newOwner.o_name = name;
        auto newOwnerPtr = std::make_shared<Owner>(newOwner);

        auto it = std::lower_bound(m_owners.begin(), m_owners.end(), newOwnerPtr, [](const std::shared_ptr<Owner> &a, const std::shared_ptr<Owner> &b) {
            return a->o_name < b->o_name;
        });

        if (it != m_owners.end() && (*it)->o_name == name) {
            (*it)->o_count++;
        } else if (it != m_owners.end() && (*it)->o_name != name) {
            m_owners.insert(it, newOwnerPtr);
            newOwnerPtr->o_count = 1;
        } else if (it == m_owners.end()) {
            m_owners.push_back(newOwnerPtr);
            newOwnerPtr->o_count = 1;
        }
    }

    void delOwner(std::string & name) {
        auto it = std::lower_bound(m_owners.begin(), m_owners.end(), name, [](const std::shared_ptr<Owner> &a, const std::string &b) {
            return a->o_name < b;
        });
        if (it != m_owners.end() && (*it)->o_name == name) {
            (*it)->o_count--;
        }
    }

    std::shared_ptr<Property> find(const std::string &region, unsigned int id) const {
        Property target;
        target.m_region = region;
        target.m_id = id;

        auto it = std::lower_bound(m_propertiesRegionID.begin(), m_propertiesRegionID.end(), target, [&](const std::shared_ptr<Property> &a, const Property &b) {
            if (a->m_region != b.m_region) return a->m_region < b.m_region;
            return a->m_id < b.m_id;
        });

        if (it != m_propertiesRegionID.end() && (*it)->m_region == region && (*it)->m_id == id) {
            return *it;
        }

        return nullptr;
    }

    std::shared_ptr<Property> find(const std::string &city, const std::string &addr) const {
        Property target;
        target.m_city = city;
        target.m_addr = addr;
        auto it = std::lower_bound(m_propertiesCityAddr.begin(), m_propertiesCityAddr.end(), target, [&](const std::shared_ptr<Property> &a, const Property &b) {
            if (a->m_city != b.m_city) return a->m_city < b.m_city;
            return a->m_addr < b.m_addr;
        });
        if (it == m_propertiesCityAddr.end() || (*it)->m_city != city || (*it)->m_addr != addr) {
            return nullptr;
        } else {
            return *it;
        }
    }

    std::shared_ptr<Owner> find(const std::string &name) const {
        auto it = std::lower_bound(m_owners.begin(), m_owners.end(), name, [&](const std::shared_ptr<Owner> &a, const std::string &b) {
            return a->o_name < b;
        });
        if (it != m_owners.end() && (*it)->o_name == name) {
            return *it;
        }
        return nullptr;
    }
};

class CIterator {
public:
    explicit CIterator(std::vector<std::shared_ptr<Property>> landRegister) : m_landRegister(std::move(landRegister)), m_currentIndex(0) {}

    bool atEnd() const {
        return m_currentIndex >= m_landRegister.size();
    }

    void next() {
        m_currentIndex++;
    }

    std::string city() const {
        return m_landRegister[m_currentIndex]->m_city;
    }

    std::string addr() const {
        return m_landRegister[m_currentIndex]->m_addr;
    }

    std::string region() const {
        return m_landRegister[m_currentIndex]->m_region;
    }

    unsigned id() const {
        return m_landRegister[m_currentIndex]->m_id;
    }

    std::string owner() const {
        return m_landRegister[m_currentIndex]->m_owner;
    }

private:
    const std::vector<std::shared_ptr<Property>> m_landRegister;
    size_t m_currentIndex;
};

CIterator CLandRegister::listByAddr() const {
    return CIterator(m_propertiesCityAddr);
}

CIterator CLandRegister::listByOwner(const std::string &owner) const {
    std::vector<std::shared_ptr<Property>> copyOwnersPtr = m_propertiesCityAddr;
    auto rem = std::remove_if(copyOwnersPtr.begin(), copyOwnersPtr.end(), [&](const std::shared_ptr<Property> &prop) {
        return !stringCaseCmp(owner, prop->m_owner);
    });
    copyOwnersPtr.erase(rem, copyOwnersPtr.end());
    std::sort(copyOwnersPtr.begin(), copyOwnersPtr.end(), [](const std::shared_ptr<Property> &a, std::shared_ptr<Property> &b) {
        return a->m_ownershipOrder < b->m_ownershipOrder;
    });
    return CIterator(copyOwnersPtr);
}

#ifndef __PROGTEST__

static void test0() {
    CLandRegister x;
    std::string owner;

    assert (x.add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.add("Prague", "Technicka", "Dejvice", 9873));
    assert (x.add("Plzen", "Evropska", "Plzen mesto", 78901));
    assert (x.add("Liberec", "Evropska", "Librec", 4552));
    assert (!x.add("Opava", "Krnovska", "Dejvice", 12345));


    CIterator i0 = x.listByAddr();
    assert (!i0.atEnd()
            && i0.city() == "Liberec"
            && i0.addr() == "Evropska"
            && i0.region() == "Librec"
            && i0.id() == 4552
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Plzen"
            && i0.addr() == "Evropska"
            && i0.region() == "Plzen mesto"
            && i0.id() == 78901
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Evropska"
            && i0.region() == "Vokovice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Technicka"
            && i0.region() == "Dejvice"
            && i0.id() == 9873
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Thakurova"
            && i0.region() == "Dejvice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (i0.atEnd());

    assert (x.count("") == 5);
    CIterator i1 = x.listByOwner("");
    assert (!i1.atEnd()
            && i1.city() == "Prague"
            && i1.addr() == "Thakurova"
            && i1.region() == "Dejvice"
            && i1.id() == 12345
            && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd()
            && i1.city() == "Prague"
            && i1.addr() == "Evropska"
            && i1.region() == "Vokovice"
            && i1.id() == 12345
            && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd()
            && i1.city() == "Prague"
            && i1.addr() == "Technicka"
            && i1.region() == "Dejvice"
            && i1.id() == 9873
            && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd()
            && i1.city() == "Plzen"
            && i1.addr() == "Evropska"
            && i1.region() == "Plzen mesto"
            && i1.id() == 78901
            && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd()
            && i1.city() == "Liberec"
            && i1.addr() == "Evropska"
            && i1.region() == "Librec"
            && i1.id() == 4552
            && i1.owner() == "");
    i1.next();
    assert (i1.atEnd());

    assert (x.count("CVUT") == 0);
    CIterator i2 = x.listByOwner("CVUT");
    assert (i2.atEnd());

    assert (x.newOwner("Prague", "Thakurova", "CVUT"));
    assert (x.newOwner("Dejvice", 9873, "CVUT"));
    assert (x.newOwner("Plzen", "Evropska", "Anton Hrabis"));
    assert (x.newOwner("Librec", 4552, "Cvut"));
    assert (x.getOwner("Prague", "Thakurova", owner) && owner == "CVUT");
    assert (x.getOwner("Dejvice", 12345, owner) && owner == "CVUT");
    assert (x.getOwner("Prague", "Evropska", owner) && owner == "");
    assert (x.getOwner("Vokovice", 12345, owner) && owner == "");
    assert (x.getOwner("Prague", "Technicka", owner) && owner == "CVUT");
    assert (x.getOwner("Dejvice", 9873, owner) && owner == "CVUT");
    assert (x.getOwner("Plzen", "Evropska", owner) && owner == "Anton Hrabis");
    assert (x.getOwner("Plzen mesto", 78901, owner) && owner == "Anton Hrabis");
    assert (x.getOwner("Liberec", "Evropska", owner) && owner == "Cvut");
    assert (x.getOwner("Librec", 4552, owner) && owner == "Cvut");
    CIterator i3 = x.listByAddr();
    assert (!i3.atEnd()
            && i3.city() == "Liberec"
            && i3.addr() == "Evropska"
            && i3.region() == "Librec"
            && i3.id() == 4552
            && i3.owner() == "Cvut");
    i3.next();
    assert (!i3.atEnd()
            && i3.city() == "Plzen"
            && i3.addr() == "Evropska"
            && i3.region() == "Plzen mesto"
            && i3.id() == 78901
            && i3.owner() == "Anton Hrabis");
    i3.next();
    assert (!i3.atEnd()
            && i3.city() == "Prague"
            && i3.addr() == "Evropska"
            && i3.region() == "Vokovice"
            && i3.id() == 12345
            && i3.owner() == "");
    i3.next();
    assert (!i3.atEnd()
            && i3.city() == "Prague"
            && i3.addr() == "Technicka"
            && i3.region() == "Dejvice"
            && i3.id() == 9873
            && i3.owner() == "CVUT");
    i3.next();
    assert (!i3.atEnd()
            && i3.city() == "Prague"
            && i3.addr() == "Thakurova"
            && i3.region() == "Dejvice"
            && i3.id() == 12345
            && i3.owner() == "CVUT");
    i3.next();
    assert (i3.atEnd());

    assert (x.count("cvut") == 3);
    CIterator i4 = x.listByOwner("cVuT");

    assert (!i4.atEnd()
            && i4.city() == "Prague"
            && i4.addr() == "Thakurova"
            && i4.region() == "Dejvice"
            && i4.id() == 12345
            && i4.owner() == "CVUT");
    i4.next();
    assert (!i4.atEnd()
            && i4.city() == "Prague"
            && i4.addr() == "Technicka"
            && i4.region() == "Dejvice"
            && i4.id() == 9873
            && i4.owner() == "CVUT");
    i4.next();
    assert (!i4.atEnd()
            && i4.city() == "Liberec"
            && i4.addr() == "Evropska"
            && i4.region() == "Librec"
            && i4.id() == 4552
            && i4.owner() == "Cvut");
    i4.next();
    assert (i4.atEnd());

    assert (x.newOwner("Plzen mesto", 78901, "CVut"));
    assert (x.count("CVUT") == 4);
    CIterator i5 = x.listByOwner("CVUT");
    assert (!i5.atEnd()
            && i5.city() == "Prague"
            && i5.addr() == "Thakurova"
            && i5.region() == "Dejvice"
            && i5.id() == 12345
            && i5.owner() == "CVUT");
    i5.next();
    assert (!i5.atEnd()
            && i5.city() == "Prague"
            && i5.addr() == "Technicka"
            && i5.region() == "Dejvice"
            && i5.id() == 9873
            && i5.owner() == "CVUT");
    i5.next();
    assert (!i5.atEnd()
            && i5.city() == "Liberec"
            && i5.addr() == "Evropska"
            && i5.region() == "Librec"
            && i5.id() == 4552
            && i5.owner() == "Cvut");
    i5.next();
    assert (!i5.atEnd()
            && i5.city() == "Plzen"
            && i5.addr() == "Evropska"
            && i5.region() == "Plzen mesto"
            && i5.id() == 78901
            && i5.owner() == "CVut");
    i5.next();
    assert (i5.atEnd());

    assert (x.del("Liberec", "Evropska"));
    assert (x.del("Plzen mesto", 78901));
    assert (x.count("cvut") == 2);
    CIterator i6 = x.listByOwner("cVuT");
    assert (!i6.atEnd()
            && i6.city() == "Prague"
            && i6.addr() == "Thakurova"
            && i6.region() == "Dejvice"
            && i6.id() == 12345
            && i6.owner() == "CVUT");
    i6.next();
    assert (!i6.atEnd()
            && i6.city() == "Prague"
            && i6.addr() == "Technicka"
            && i6.region() == "Dejvice"
            && i6.id() == 9873
            && i6.owner() == "CVUT");
    i6.next();
    assert (i6.atEnd());

    assert (x.add("Liberec", "Evropska", "Librec", 4552));
}

static void test1() {
    CLandRegister x;
    std::string owner;

    assert (x.add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.add("Prague", "Technicka", "Dejvice", 9873));
    assert (!x.add("Prague", "Technicka", "Hradcany", 7344));
    assert (!x.add("Brno", "Bozetechova", "Dejvice", 9873));
    assert (!x.getOwner("Prague", "THAKUROVA", owner));
    assert (!x.getOwner("Hradcany", 7343, owner));
    CIterator i0 = x.listByAddr();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Evropska"
            && i0.region() == "Vokovice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Technicka"
            && i0.region() == "Dejvice"
            && i0.id() == 9873
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Thakurova"
            && i0.region() == "Dejvice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (i0.atEnd());

    assert (x.newOwner("Prague", "Thakurova", "CVUT"));
    assert (!x.newOwner("Prague", "technicka", "CVUT"));
    assert (!x.newOwner("prague", "Technicka", "CVUT"));
    assert (!x.newOwner("dejvice", 9873, "CVUT"));
    assert (!x.newOwner("Dejvice", 9973, "CVUT"));
    assert (!x.newOwner("Dejvice", 12345, "CVUT"));
    assert (x.count("CVUT") == 1);
    CIterator i1 = x.listByOwner("CVUT");
    assert (!i1.atEnd()
            && i1.city() == "Prague"
            && i1.addr() == "Thakurova"
            && i1.region() == "Dejvice"
            && i1.id() == 12345
            && i1.owner() == "CVUT");
    i1.next();
    assert (i1.atEnd());

    assert (!x.del("Brno", "Technicka"));
    assert (!x.del("Karlin", 9873));
    assert (x.del("Prague", "Technicka"));
    assert (!x.del("Prague", "Technicka"));
    assert (!x.del("Dejvice", 9873));
}

int main() {
    test0();
    test1();
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */