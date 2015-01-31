CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ise-web-helper-agent CXX)

SET(ISE_SRCS
    src/main.cpp
    src/common.cpp
    src/ise.cpp
    src/packages.cpp
    src/web_container.cpp
    src/web_helper_agent.cpp
    src/web_helper_agents/websocket.cpp
    src/web_helper_agents/direct.cpp
)

SET(ISE_PACKAGE ${PROJECT_NAME})
SET(ISE_PKGNAME ${PACKAGE})
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(ISE_LIBDIR "${PREFIX}/lib/scim-1.0/1.4.0/Helper")
SET(MANIFEST_DIR "${PREFIX}/share/packages")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src/web_helper_agents
    ${CMAKE_SOURCE_DIR}/src )

INCLUDE(FindPkgConfig)
pkg_check_modules(ISE_PKGS REQUIRED
        elementary
        dlog
        ecore-imf
        isf
        vconf
        libwebsockets
        pkgmgr-info
        capi-appfw-package-manager
        )

FOREACH(flag ${ISE_PKGS_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -finstrument-functions")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

ADD_DEFINITIONS("-DPACKAGE=\"${ISE_PACKAGE}\"")
ADD_DEFINITIONS("-DPACKAGE_NAME=\"${ISE_PKGNAME}\"")

#ADD_EXECUTABLE(${PROJECT_NAME} ${ISE_SRCS})
ADD_LIBRARY(${PROJECT_NAME} SHARED ${ISE_SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ISE_PKGS_LDFLAGS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${ISE_LIBDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.efl DESTINATION
            /etc/smack/accesses.d/)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.xml DESTINATION
            ${MANIFEST_DIR})

###########################################
# Web container plugin so #
###########################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(web-container CXX)

SET(WEB_CONTAINER_SRCS
    src/packages.cpp
    src/web_container_plugin.cpp
)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(ISF_LIBDIR "${PREFIX}/lib/scim-1.0/")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src )

INCLUDE(FindPkgConfig)
pkg_check_modules(WEB_CONTAINER_PKGS REQUIRED
        evas
        dlog
        ewebkit2
        wrt-core
        dpl-efl
        )

FOREACH(flag ${WEB_CONTAINER_PKGS_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall -std=c++0x")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -finstrument-functions")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${WEB_CONTAINER_SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${WEB_CONTAINER_PKGS_LDFLAGS})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${ISF_LIBDIR})
