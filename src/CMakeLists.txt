add_executable(list main.cpp)

target_link_libraries(list PUBLIC liblogs liblist)

target_include_directories(list PRIVATE ${CMAKE_CURRENT_LIST_DIR})