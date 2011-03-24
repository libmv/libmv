# Copyright (c) 2007-2011 libmv authors.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

SET(LIBMV_AUTHORS "Libmv Team.")

SET(CPACK_PACKAGE_NAME "libmv")
SET(CPACK_PACKAGE_VERSION ${LIBMV_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${LIBMV_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${LIBMV_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${LIBMV_VERSION_PATCH})

# The libmv binary package must contain: bin/ lib/*.so(dll)
IF (WIN32)
  SET(CPACK_GENERATOR "NSIS;ZIP")
ELSE (WIN32)
  SET(CPACK_GENERATOR "TBZ2;DEB;RPM")
ENDIF(WIN32)
SET(CPACK_IGNORE_FILES  ".svn/;.kdev4/;build/;bin-opt/;bin-dbg/;~$;${CPACK_SOURCE_IGNORE_FILES}")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${LIBMV_VERSION}")
IF (INSTALL_SOURCE STREQUAL ON)
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${LIBMV_VERSION}-full")
ENDIF (INSTALL_SOURCE STREQUAL ON)
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

# The libmv source package must contain: binary + src/ lib/*.a(lib) contrib/ extras/ doc/ ?
IF (WIN32)
  SET(CPACK_SOURCE_GENERATOR "ZIP")
ELSE (WIN32)
  SET(CPACK_SOURCE_GENERATOR "TBZ2;DEB;RPM")
ENDIF(WIN32)
SET(CPACK_SOURCE_IGNORE_FILES  ".svn/;.kdev4/;build/;bin-opt/;bin-dbg/;bin/;~$;${CPACK_SOURCE_IGNORE_FILES}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${LIBMV_VERSION}-dev")

# SET(CPACK_PACKAGE_EXECUTABLES "the_tools;Tool description")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1)")
# TODO(julien) add third_parties: libpthread-stubs0, libjpeg62, (zlib?, png?) 

SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Libmv-${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "libmv-${LIBMV_VERSION}")

SET(CPACK_PACKAGE_DESCRIPTION "A Structure from Motion library")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Structure from Motion library")

SET(CPACK_PACKAGE_CONTACT "${LIBMV_AUTHORS} <libmv-devel@googlegroups.com>")
SET(CPACK_PACKAGE_VENDOR ${LIBMV_AUTHORS})

SET(CPACK_RESOURCE_FILE_LICENSE "${LIBMV_SOURCE_DIR}/../LICENSE")
SET(CPACK_RESOURCE_FILE_README "${LIBMV_SOURCE_DIR}/../README")

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# #set(CPACK_PACKAGE_ICON "${LIBMV_SOURCE_DIR}\\\\libmv-logo.png")
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr \\\${WriteEnvStr_RegKey} \\\"LIBMV_ROOT\\\" $INSTDIR")
# Tell CPack all of the components to install. The "ALL"
# refers to the fact that this is the set of components that
# will be included when CPack is instructed to put everything
# into the binary installer (the default behavior).
set(CPACK_COMPONENTS_ALL applications libraries headers sources documentation)
set(CPACK_MONOLITHIC_INSTALL ON)

# Set the displayed names for each of the components to install.
# These will be displayed in the list of components inside the installer.
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "libmv-tools")
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_SOURCES_DISPLAY_NAME "C++ Sources Files")
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")

# Provide descriptions for each of the components to install.
# When the user hovers the mouse over the name of a component, 
# the description will be shown in the "Description" box in the
# installer. If no descriptions are provided, the "Description" 
# box will be removed.
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION 
  "Some useful applications that use libmv")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION
  "Static libraries used to build programs with libmv")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION
  "C/C++ header files for use with libmv")
set(CPACK_COMPONENT_SOURCES_DESCRIPTION
  "C/C++ sources files for building libmv")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION
  "Development documentation")

# Put the components into two different groups: "Runtime" and "Development"
set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
set(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
set(CPACK_COMPONENT_HEADERS_GROUP "Development")
set(CPACK_COMPONENT_SOURCES_GROUP "Development")
set(CPACK_COMPONENT_DOCUMENTATION_GROUP "Development")

# Expand the "Development" group by default, since we have so few components.
# Also, provide this group with a description.
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_EXPANDED ON)
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION
  "All of the tools you'll ever need to develop software")

# It doesn't make sense to install the headers without the libraries
# (because you could never use the headers!), so make the headers component
# depend on the libraries component.
set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

# Create two installation types with pre-selected components.
# The "Developer" installation has just the library and headers,
# while the "Full" installation has everything.
set(CPACK_ALL_INSTALL_TYPES Full Developer)
set(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")
set(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full)
set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full)
set(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Full)
set(CPACK_COMPONENT_SOURCES_INSTALL_TYPES Developer Full)
set(CPACK_COMPONENT_DOCUMENTATION_INSTALL_TYPES Developer Full)

# TODO(julien) A post installation that 
#  - copy pthread.dll in $ENV{SystemRoot}/system32
#  - copy FindLibmv.cmake in ???/CMake??/Modules/
#  - ask for the user for relogging/reboot 
#set(CPACK_NSIS_MUI_FINISHPAGE_RUN CopyPthreadWinSystem.bat)

# TODO(julien) Bundle (Mac OS)
# see http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

INCLUDE(CPack)
