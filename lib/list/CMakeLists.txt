add_library(liblist list.cpp cringe.cpp)

target_link_libraries(liblist PRIVATE liblogs libgraph)

target_include_directories(liblist PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})