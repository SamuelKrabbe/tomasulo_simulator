# Install script for directory: /home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/opt/systemc")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "doc" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/systemc" TYPE FILE FILES
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/AUTHORS"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/ChangeLog"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/INSTALL"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/cmake/INSTALL_USING_CMAKE"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/LICENSE"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/NEWS"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/NOTICE"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/README"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/RELEASENOTES"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake"
         "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/CMakeFiles/Export/f84e08586e04e413dbda18fea2defa39/SystemCLanguageTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/CMakeFiles/Export/f84e08586e04e413dbda18fea2defa39/SystemCLanguageTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/CMakeFiles/Export/f84e08586e04e413dbda18fea2defa39/SystemCLanguageTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/SystemCLanguageConfig.cmake"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/SystemCLanguageConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCTLM" TYPE FILE FILES
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/SystemCTLMConfig.cmake"
    "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/SystemCTLMConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/docs/cmake_install.cmake")
  include("/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/src/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/samuel-krabbe/Code/Arq_II/Trab_2_tomasulo_simulator/systemc-2.3.3/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
