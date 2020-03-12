# Only support Release and Debug build types
set(CMAKE_CONFIGURATION_TYPES "Release" "Debug")

# Set default build type to Debug
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Ensure the build type is set
if(NOT CMAKE_BUILD_TYPE IN_LIST CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "Build type ${CMAKE_BUILD_TYPE} is not supported")
endif()
