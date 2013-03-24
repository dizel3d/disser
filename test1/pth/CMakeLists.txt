add_executable(test_pth main.cpp)
target_link_libraries(test_pth pth)
set_target_properties(test_pth PROPERTIES OUTPUT_NAME "test") 
