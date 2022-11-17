project(zabbix-module-sysrepo C)
cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})
find_package(PkgConfig REQUIRED)
pkg_search_module(SYSREPO REQUIRED sysrepo>=2.0.0)

set(ZABBIX_DIR CACHE STRING "path to Zabbix sources")

add_library(zbx_sysrepo SHARED src/sysrepo.c)
target_include_directories(zbx_sysrepo PUBLIC
    ${SYSREPO_INCLUDE_DIRS} ${ZABBIX_DIR}/include)

set_target_properties(zbx_sysrepo PROPERTIES PREFIX "")

target_link_libraries(zbx_sysrepo
    ${SYSREPO_LINK_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS zbx_sysrepo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/modules)
