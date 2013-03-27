add_executable(test3_pthread main.cpp)
target_link_libraries(test3_pthread pthread)
set_target_properties(test3_pthread PROPERTIES OUTPUT_NAME "pthread")

add_executable(test3_pth main.cpp)
target_link_libraries(test3_pth ${pthread})
set_target_properties(test3_pth PROPERTIES OUTPUT_NAME "pth")

configure_file("test.sh" "test.sh" COPYONLY)
