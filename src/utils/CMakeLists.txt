file(GLOB HEADERS
        "${TPFAAS_INCLUDE_DIR}/utils/*.h"
        )

file(GLOB SRC_FILES
        "${TPFAAS_SOURCE_DIR}/utils/*.cpp"
        )

set(LIB_FILES ${HEADERS} ${SRC_FILES})
tpfaas_private_lib(utils "${LIB_FILES}")

