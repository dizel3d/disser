add_executable(test_pthread main.cpp)
target_link_libraries(test_pthread pthread)
set_target_properties(test_pthread PROPERTIES OUTPUT_NAME "test") 
