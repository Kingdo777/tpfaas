set(LIBS fcgi RFIT libWAVM)

function(test_function function_name)
    add_executable(${function_name} ${function_name}.cpp)
    target_link_libraries(${function_name} ${LIBS})
endfunction()

test_function(test_ucontext)
test_function(test_resource)
test_function(test_function)
