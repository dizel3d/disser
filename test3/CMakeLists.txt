add_executable(test3_pthread main.cpp test_cases.cpp timer.cpp)
target_link_libraries(test3_pthread pthread)
set_target_properties(test3_pthread PROPERTIES OUTPUT_NAME "pthread")

add_executable(test3_pth main.cpp test_cases.cpp timer.cpp)
target_link_libraries(test3_pth ${pthread})
set_target_properties(test3_pth PROPERTIES OUTPUT_NAME "pth")

add_executable(test3_st st.cpp test_cases.cpp timer.cpp)
include_directories(${st_INCLUDE_DIRS})
target_link_libraries(test3_st ${st})
set_target_properties(test3_st PROPERTIES OUTPUT_NAME "st")

configure_file("test.sh" "test.sh" COPYONLY)

add_custom_target(test3_mex ALL mex -o test ${CMAKE_CURRENT_SOURCE_DIR}/mex.c DEPENDS mex.c)
configure_file("test.m" "test.m" COPYONLY)