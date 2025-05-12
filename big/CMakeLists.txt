cmake_minimum_required(VERSION 3.23)
project(big)

set(CMAKE_CXX_STANDARD 20)


add_executable(big test.cpp CSpreadsheet.cpp CSpreadsheet.h CPos.cpp CPos.h MyExprBuilder.cpp MyExprBuilder.h CData.cpp CData.h CTreeNode.cpp CTreeNode.h utils.cpp utils.h)
add_executable(onefile file.cpp)

target_link_libraries(big -L../x86_64-linux-gnu)
target_link_libraries(big -lexpression_parser)

target_link_libraries(onefile -L../x86_64-linux-gnu)
target_link_libraries(onefile -lexpression_parser)
