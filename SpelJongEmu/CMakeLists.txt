cmake_minimum_required(VERSION 3.1)

# Rename this variable to change the project name
SET(PROJECT_NAME Speljongen)

# Set up the project
project(${PROJECT_NAME})

# Set the path to our find modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Some default variables which the user may change
SET(CMAKE_BUILD_TYPE        Release CACHE STRING  "Choose the type of build (Debug or Release)")

# We're using c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Apple doesn't like static constexpr if c++17 isn't enabled
if (APPLE)
 set(CMAKE_CXX_STANDARD 17)
endif()

# enable some warnings in debug builds with gcc/clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wreorder")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wreorder -Wheader-guard")
endif()

# UI libs for NFD
if (APPLE)
find_library(CORESERVICES_LIBRARY CoreServices)
find_library(APPKIT AppKit)
elseif(UNIX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
endif()

# Only works with SFML version 2.4 and above
SET(SFML_MIN_VERSION 2.4)
find_package(SFML ${SFML_MIN_VERSION} REQUIRED graphics window system network)

# We're using SDL2 for audio, because hum...
find_package(SDL2 REQUIRED)

#required by ImGui
find_package(OpenGL REQUIRED)

# X11 is required on unices
if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
endif()

# Additional include directories
include_directories(
  ${SFML_INCLUDE_DIR} 
  ${SDL2_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/include)

if(X11_FOUND)
  include_directories(${X11_INCLUDE_DIRS})
endif()

if(UNIX)
  include_directories(${GTK_INCLUDE_DIRS})
  link_directories(${GTK_LIBRARY_DIRS})
endif()

# Project source files
add_subdirectory(src)

# platform specific for file dialogue
if(APPLE)
  SET(NFD_SRC 
    src/nfd/nfd_common.c
    src/nfd/nfd_cocoa.m)
elseif(UNIX)
  SET(NFD_SRC 
    src/nfd/nfd_common.c
    src/nfd/nfd_gtk.c)
elseif(WIN32)
  SET(NFD_SRC 
    src/nfd/nfd_common.c
    src/nfd/nfd_win.cpp)
endif()

# Create the actual executable (PROJECT_SRC variable is set inside previous steps)
If (APPLE)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

  set_source_files_properties( 
    ${CMAKE_SOURCE_DIR}/assets PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/macOS/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_SRC} ${NFD_SRC} ${CMAKE_SOURCE_DIR}/assets ${CMAKE_SOURCE_DIR}/macOS/icon.icns)
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.icns)
else()
  add_executable(${PROJECT_NAME} ${PROJECT_SRC} ${NFD_SRC})
endif(APPLE)

# Linker settings
target_link_libraries(${PROJECT_NAME}
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  ${SDL2_LIBRARY}
  ${OPENGL_LIBRARIES})

if(X11_FOUND)
  target_link_libraries(${PROJECT_NAME}
    ${X11_LIBRARIES})
endif()

# NFD deps
if (APPLE)
target_link_libraries(${PROJECT_NAME} ${CORESERVICES_LIBRARY} ${APPKIT})
elseif(UNIX)
target_link_libraries(${PROJECT_NAME} ${GTK_LIBRARIES})
endif()

# Install executable
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION .
  BUNDLE DESTINATION ${CMAKE_SOURCE_DIR})

# Install assets folder except on Apple, where it's included in the app bundle
if (NOT APPLE)
  install(DIRECTORY assets
  DESTINATION .)
endif()

#Platform specific packaging
if(APPLE)
	#set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME})
    install(CODE " include(BundleUtilities)
		 fixup_bundle(${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.app \"\" \"\")
     verify_app(${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.app)")
    SET(CPACK_GENERATOR "DragNDrop")
endif()

include(CPack)