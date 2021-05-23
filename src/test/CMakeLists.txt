set(LIBS fcgi RFIT libWAVM tpfaas-endpoint utils)
set(INCLUDES
        ${FAABRIC_INCLUDE_DIR}
        ${TPFAAS_INCLUDE_DIR}
        ${WAVM_SOURCE_DIR}/Include
        ${RapidJSON_SOURCE_DIR}/include)

function(test_function function_name)
    add_executable(${function_name} ${function_name}.cpp)
    target_link_libraries(${function_name} ${LIBS})
    target_include_directories(${function_name} PUBLIC ${INCLUDES})
endfunction()

test_function(test_ucontext)
test_function(test_resource)
test_function(test_function)
test_function(test_endpoint)
