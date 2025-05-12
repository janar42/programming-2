//#include "CPos.h"
//
//CPos::CPos(std::string_view str) {
//    size_t size = str.size();
//    size_t pos;
//    bool letters = true;
//    std::string row;
//
//    if (std::isdigit(str[0])) {
//        throw std::invalid_argument("Invalid argument\n.");
//    }
//
//    for (size_t i = 0; i < size; i++) {
//        if (letters) {
//            if (!isalpha(str[i])) {
//                if (!std::isdigit(str[i])) {
//                    throw std::invalid_argument("Invalid argument\n.");
//                } else {
//                    letters = false;
//                    pos = i;
//                    row += str[i];
//                }
//            }
//        } else {
//            if (!std::isdigit(str[i])) {
//                throw std::invalid_argument("Invalid argument\n.");
//            } else {
//                row += str[i];
//            }
//        }
//    }
//
//    if (letters) {
//        throw std::invalid_argument("Invalid argument\n.");
//    }
//
//    std::string lower;
//    for (char c : str.substr(0,pos)) {
//        lower += tolower(c);
//    }
//
//    m_column = lower;
//    m_row = std::stoi(row);
//}
//
//

