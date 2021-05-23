#file(GLOB HEADERS "${FAABRIC_INCLUDE_DIR}/faabric/endpoint/*.h")
#
#set(LIB_FILES
#        Endpoint.cpp
#        FaabricEndpoint.cpp
#        TpfaasEndpointHandler.cpp
#        ${HEADERS}
#        )
#
#faabric_lib(endpoint "${LIB_FILES}")
#
##add_dependencies(endpoint pistache_ext)
#target_link_directories(endpoint PUBLIC ${CMAKE_INSTALL_PREFIX}/lib)
#target_link_libraries(endpoint pistache pthread util)

file(GLOB HEADERS
        "${TPFAAS_INCLUDE_DIR}/endpoint/*.h"
        )

file(GLOB SRC_FILES
        "${TPFAAS_SOURCE_DIR}/endpoint/*.cpp"
        )


set(LIB_FILES ${HEADERS} ${SRC_FILES})
tpfaas_private_lib(tpfaas-endpoint "${LIB_FILES}")
target_link_libraries(tpfaas-endpoint pthread pistache)