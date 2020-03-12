# descript the systems
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "arm")

# specify toolchain binaries
set(TOOLCHAIN_PREFIX "arm-none-eabi-")
find_program(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
find_program(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++")
find_program(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy")
find_program(CMAKE_SIZE_UTIL "${TOOLCHAIN_PREFIX}size")
unset(TOOLCHAIN_PREFIX)

# get the base dir of the toolchain
get_filename_component(TOOLCHAIN_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
get_filename_component(TOOLCHAIN_BASE_DIR "${TOOLCHAIN_BIN_DIR}" DIRECTORY)

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_BASE_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM "NEVER")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY "ONLY")
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE "ONLY")

unset(TOOLCHAIN_BASE_DIR)
unset(TOOLCHAIN_BIN_DIR)

# compile flags
set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fshort-enums")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
unset(COMMON_FLAGS)
