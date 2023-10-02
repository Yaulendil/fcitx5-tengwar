add_library(fcitx-tengwar SHARED tengwar.cpp)
target_link_libraries(fcitx-tengwar PRIVATE Fcitx5::Core)


# Set up ExternalProject module.
include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/rust)

# Add rustlib as a CMake target and dependency.
ExternalProject_Add(
    rustlib
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/tengwar_wrapper"
    INSTALL_COMMAND ""
    LOG_BUILD ON
)
add_dependencies(fcitx-tengwar rustlib)

# Specify link libraries.
target_link_libraries(fcitx-tengwar PRIVATE
    debug "${CMAKE_SOURCE_DIR}/tengwar_wrapper/target/debug/libtengwar_wrapper.a"
    optimized "${CMAKE_SOURCE_DIR}/tengwar_wrapper/target/release/libtengwar_wrapper.a"
)


# Continue with main build.
set_target_properties(fcitx-tengwar PROPERTIES PREFIX "")
install(TARGETS fcitx-tengwar DESTINATION "${FCITX_INSTALL_LIBDIR}/fcitx5")

# Set up config files.
configure_file(tengwar-addon.conf tengwar-addon.conf)
install(FILES "tengwar-addon.conf" RENAME tengwar.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")
install(FILES "tengwar-input.conf" RENAME tengwar.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod")
