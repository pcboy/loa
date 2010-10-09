PROJECT(loa)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_BUILD_TYPE Release)
FIND_PACKAGE(PkgConfig REQUIRED)
pkg_check_modules (GTK2 gtk+-2.0)
pkg_check_modules (WEBKIT webkit-1.0)
pkg_check_modules (LIBSOUP libsoup-2.4)
SET(SRCS
    ../src/main.c
    ../src/loa.c
    )
SET(CMAKE_C_FLAGS "-W -Wall -Wextra")
INCLUDE_DIRECTORIES(
        ${LIBSOUP_INCLUDE_DIRS}
        ${WEBKIT_INCLUDE_DIRS}
        ${GTK2_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../inc/
        )
ADD_EXECUTABLE(loa ${SRCS})
TARGET_LINK_LIBRARIES(loa ${WEBKIT_LIBRARIES} ${GTK2_LIBRARIES}
    ${LIBSOUP_LIBRARIES})
INSTALL(TARGETS loa DESTINATION bin)
