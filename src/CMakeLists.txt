add_executable(blink blink.cpp)
target_link_libraries(blink PUBLIC nrf5_sdk)
register_firmware_target(blink)

add_executable(advertising advertising.cpp)
target_link_libraries(advertising PUBLIC nrf5_sdk)
register_firmware_target(advertising)

add_executable(pairing pairing.cpp)
target_link_libraries(pairing PUBLIC nrf5_sdk)
register_firmware_target(pairing)

add_executable(characteristics characteristics.cpp)
target_link_libraries(characteristics PUBLIC nrf5_sdk)
register_firmware_target(characteristics)
