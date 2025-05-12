#include "CSpreadsheet.h"


CSpreadsheet::CSpreadsheet() = default;

CSpreadsheet::CSpreadsheet(const CSpreadsheet & other) {
    for (const auto & cell : other.m_sheet) {
        CPos newPos(cell.first);
        CData * newData = cell.second->copy();
        m_sheet[newPos] = newData;
    }
}

CSpreadsheet &CSpreadsheet::operator=(const CSpreadsheet &other) {
    if (this != &other) {
        for (auto & cell : m_sheet) {
            delete cell.second;
        }

        m_sheet.clear();

        for (const auto & cell : other.m_sheet) {
            CPos newPos(cell.first);
            CData * newData = cell.second->deepCopy(this);
            m_sheet[newPos] = newData;
        }
    }

    return *this;
}

CSpreadsheet::~CSpreadsheet() {
    for (auto & cell : m_sheet) {
        delete cell.second;
    }
    m_sheet.clear();
}

bool CSpreadsheet::load(std::istream & is) {
    if (is.fail()) {
        return false;
    }

    int numOfCells = readInt(is);
    int i = 0;
    int colLen;
    size_t controlSum = 0;

    try {
        while (i < numOfCells) {
            colLen = readInt(is);
            if (colLen == -1) {
                break;
            }
            std::string cellCol = readString(is, colLen);

            controlSum += cellCol.size();
            for (const char & c : cellCol) {
                controlSum += c;
            }

            int rowLen = readInt(is);
            if (rowLen == -1) {
                break;
            }
            std::string cellRow = readString(is, rowLen);

            controlSum += cellRow.size();
            for (const char & c : cellRow) {
                controlSum += c;
            }

            int contentLen = readInt(is);
            if (contentLen == -1) {
                break;
            }
            std::string content = readString(is, contentLen);

            controlSum += content.size();
            for (const char & c : content) {
                controlSum += c;
            }

            CPos cell(cellCol + cellRow);
            setCell(cell, content);
            i++;
        }

    } catch (const std::exception & e) {
        return false;
    }

    if (readInt(is) != controlSum) {
        return false;
    }

    return true;
}

bool CSpreadsheet::save(std::ostream & os) const {
    size_t numOfCells = m_sheet.size();
    os.write((char *)&numOfCells, sizeof(int));

    size_t controlSum = 0;
    for (const auto & cell : m_sheet) {
        strToBin(os, cell.first.getCol(), controlSum);
        strToBin(os, std::to_string(cell.first.getRow()), controlSum);
        std::string cellValue;
        if (std::holds_alternative<double>(cell.second->getCell())) {
            cellValue = std::to_string(std::get<double>(cell.second->getCell()));
            strToBin(os, cellValue, controlSum);
        } else {
            strToBin(os, std::get<std::string>(cell.second->getCell()), controlSum);
        }
    }
    os.write((char *)&controlSum, sizeof(int));

    return true;
}

bool CSpreadsheet::setCell(CPos pos, std::string contents) {
    CValue value = convertToCValue(contents);
    if (std::holds_alternative<std::monostate>(value)) {
        return false;
    }

    if (std::holds_alternative<double>(value)) {
        CDouble dbl;
        dbl.m_value = std::get<double>(value);
        m_sheet[pos] = dbl.copy();
        return true;

    } else if (std::holds_alternative<std::string>(value)) {
        std::string strValue = std::get<std::string>(value);
        if (strValue[0] == '=') {
            CExpression expr(strValue, this);
            MyExprBuilder builder(this);
            try {
                parseExpression(strValue, builder);
            } catch (const std::exception & e) {
                return false;
            }
            m_sheet[pos] = expr.copy();
            return true;

        } else {
            CString str;
            str.m_value = strValue;
            m_sheet[pos] = str.copy();
            return true;
        }
    }
}

CValue CSpreadsheet::getValue(CPos pos) {
    auto it = m_sheet.find(pos);

    std::unordered_set<CPos> visited;
    visited.insert(pos);

    if (it != m_sheet.end()) {
        return getValueSet(pos, visited);
    }
    return {};
}

CValue CSpreadsheet::getValueSet(CPos pos, std::unordered_set<CPos> visited) {
    auto it = m_sheet.find(pos);

    if (it != m_sheet.end()) {
        return it->second->evalCell(pos, visited);
    }

    return {};
}

void CSpreadsheet::copyRect(CPos dst, CPos src, int w, int h) {
    int srcX = strToIntCol(src.m_column);
    int dstX = strToIntCol(dst.m_column);
    std::pair<int, int> moveBy = {dstX - srcX, dst.getRow() - src.getRow()};

    std::map<CPos, CData *> tmpMap;

    for (const auto & cell : m_sheet) {
        if (cell.first.getCol() >= src.getCol() && cell.first.getCol() < addToCol(src.m_column, w) && cell.first.getRow() >= src.getRow() && cell.first.getRow() < src.getRow() + h) {
            CPos newPos;
            auto sourceStr = cell.first.getCol();
            newPos.m_column = addToCol(sourceStr, moveBy.first);
            newPos.m_row = cell.first.getRow() + moveBy.second;
            tmpMap[newPos] = cell.second->moveRect(moveBy.first, moveBy.second);
        }
    }

    for (const auto & tmpCell : tmpMap) {
        m_sheet[tmpCell.first] = tmpCell.second;
    }

}

CValue CSpreadsheet::convertToCValue(const std::string & contents) {
    if (contents.empty()) {
        return std::monostate();
    }

    std::string value = contents;
    try {
        size_t pos;
        double number = std::stod(contents, &pos);
        if (pos == contents.size()) {
            return number;
        }
    } catch (const std::invalid_argument &) {}

    return value;
}

void CSpreadsheet::strToBin(std::ostream & os, const std::string & str, size_t &controlSum) const {
    size_t strSize = str.size();
    os.write((char *)&strSize, sizeof(int));
    controlSum += strSize;

    for (const char & c : str) {
        os.write((char *) &c, sizeof(char));
        controlSum += c;
    }
}

std::string CSpreadsheet::readString(std::istream & is, int numberOfChars) const {
    if (is.eof()) {
        throw std::exception();
    }
    std::vector<char> charVector(numberOfChars);
    is.read(charVector.data(), numberOfChars);
    if (!is) {
        throw std::exception();
    }

    std::string str(charVector.begin(), charVector.end());
    return str;
}

size_t CSpreadsheet::readInt(std::istream & is) const {
    if (is.eof()) {
        return -1;
    }
    int intValue;
    is.read(reinterpret_cast<char*>(&intValue), sizeof(int));
    if (!is) {
        return -1;
    }

    return intValue;
}

