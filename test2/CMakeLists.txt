add_executable(test2 main.cpp)
target_link_libraries(test2 pth pthread)
set_target_properties(test2 PROPERTIES OUTPUT_NAME "test") 
