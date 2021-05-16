option(STEAL_INSTANCE "if open the STEAL_INSTANCE" OFF)

file(GLOB HEADERS
        "${TPFAAS_INCLUDE_DIR}/RFIT/R/*.h"
        "${TPFAAS_INCLUDE_DIR}/RFIT/F/*.h"
        "${TPFAAS_INCLUDE_DIR}/RFIT/I/*.h"
        "${TPFAAS_INCLUDE_DIR}/RFIT/T/*.h"
        )

file(GLOB SRC_FILES
        "${TPFAAS_SOURCE_DIR}/RFIT/R/*.cpp"
        "${TPFAAS_SOURCE_DIR}/RFIT/F/*.cpp"
        "${TPFAAS_SOURCE_DIR}/RFIT/I/*.cpp"
        "${TPFAAS_SOURCE_DIR}/RFIT/T/*.cpp"
        )
if (STEAL_INSTANCE)
    add_definitions(-DSTEAL_INSTANCE)
endif ()

set(LIB_FILES ${HEADERS} ${SRC_FILES})
tpfaas_private_lib(RFIT "${LIB_FILES}")
target_link_libraries(RFIT m pthread faabric libWAVM)

