include("${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/sitkCMakeInit.cmake")
cmake_minimum_required(
  VERSION ${SITK_CMAKE_MINIMUM_REQUIRED_VERSION}...${SITK_CMAKE_POLICY_VERSION}
)

project(SimpleITK_CSharp)

include(../../CMake/sitkProjectLanguageCommon.cmake NO_POLICY_SCOPE)

# Find C#
find_package(CSharp REQUIRED)
include(${CSHARP_USE_FILE})

include_directories(${SimpleITK_INCLUDE_DIRS})

set_source_files_properties(
  SimpleITK.i
  PROPERTIES
    CPLUSPLUS
      ON
)

# CSharp version requirements: http://msdn.microsoft.com/en-us/library/system.reflection.assemblyversionattribute.aspx
# major.minor[.build[.revision]] where all components are 16-bit unsigned integers
set(_build 0)
if(
  DEFINED
    SimpleITK_VERSION_POST
  AND
    NOT
      "${SimpleITK_VERSION_POST}"
        STREQUAL
        ""
)
  math(EXPR _build "${SimpleITK_VERSION_POST}")
elseif(
  DEFINED
    SimpleITK_VERSION_DEV
  AND
    NOT
      "${SimpleITK_VERSION_DEV}"
        STREQUAL
        ""
)
  math(EXPR _build "32768+${SimpleITK_VERSION_DEV}")
endif()
if(_build GREATER 65535)
  message(
    WARNING
    "CSharp build component overflowed, setting to 65535 instead of ${_build}."
  )
  set(_build 65535)
endif()

set(_revision 0)
if(DEFINED SimpleITK_VERSION_PATCH)
  math(EXPR _revision "${SimpleITK_VERSION_PATCH}<<8")
  if(DEFINED SimpleITK_VERSION_TWEAK)
    math(EXPR _revision "${_revision}+${SimpleITK_VERSION_TWEAK}")
  endif()
endif()
if(_revision GREATER 65535)
  message(
    WARNING
    "CSharp revision component overflowed, setting to 65535 instead of ${_revision}."
  )
  set(_revision 65535)
endif()

set(
  SimpleITK_VERSION_CSHARP_AssemblyVersion
  "${SimpleITK_VERSION_MAJOR}.${SimpleITK_VERSION_MINOR}.${_build}.${_revision}"
)

# Make sure the nested directory structure exists
set(
  CSHARP_SOURCE_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/CSharpSources
  CACHE INTERNAL
  ""
)
set(
  CSHARP_BINARY_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/CSharpBinaries
  CACHE INTERNAL
  ""
)
file(MAKE_DIRECTORY ${CSHARP_SOURCE_DIRECTORY})
file(MAKE_DIRECTORY ${CSHARP_BINARY_DIRECTORY})

# Create swig target
set(CMAKE_SWIG_OUTDIR ${CSHARP_SOURCE_DIRECTORY})
if(UNIX)
  set(
    CMAKE_SWIG_FLAGS
    -dllimport
    \"libSimpleITKCSharpNative\"
  )
else(WIN32)
  set(
    CMAKE_SWIG_FLAGS
    -dllimport
    \"SimpleITKCSharpNative\"
  )
endif()
set(
  CMAKE_SWIG_FLAGS
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -namespace
  \"itk.simple\"
  ${CMAKE_SWIG_GLOBAL_FLAGS}
  ${CMAKE_SWIG_FLAGS}
)
set(
  SWIG_MODULE_SimpleITKCSharpNative_EXTRA_DEPS
  ${SWIG_EXTRA_DEPS}
  ${CMAKE_CURRENT_SOURCE_DIR}/CSharp.i
  ${CMAKE_CURRENT_SOURCE_DIR}/CSharpTypemapHelper.i
)
swig_add_module(SimpleITKCSharpNative csharp SimpleITK.i)
target_link_libraries(
  ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
  ${SimpleITK_LIBRARIES}
)
set_target_properties(
  ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
      ${CSHARP_BINARY_DIRECTORY}
)
if(APPLE)
  set_target_properties(
    ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
    PROPERTIES
      PREFIX
        "lib"
  )
  set_target_properties(
    ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
    PROPERTIES
      SUFFIX
        ".dylib"
  )
elseif(UNIX)
  set_target_properties(
    ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
    PROPERTIES
      PREFIX
        "lib"
  )
  set_target_properties(
    ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
    PROPERTIES
      SUFFIX
        ".so"
  )
else(WIN32)
  set_target_properties(
    ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
    PROPERTIES
      PREFIX
        ""
  )
  set_target_properties(
    ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
    PROPERTIES
      SUFFIX
        ".dll"
  )
  foreach(CMAKE_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CMAKE_CONFIGURATION_TYPE} CMAKE_CONFIGURATION_TYPE)
    set_target_properties(
      ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
      PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE}
          "${CSHARP_BINARY_DIRECTORY}"
    )
    set_target_properties(
      ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
      PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE}
          "${CSHARP_BINARY_DIRECTORY}"
    )
  endforeach()
endif()
sitk_strip_target( ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME} )

# These CSharp files are generated by SWIG, they are not there
# during the first build. But the charp_add_library will add them as
# a dependency when the exists. This is needed for CMP0058
file(GLOB _files "${CSHARP_SOURCE_DIRECTORY}/*.cs")
add_custom_command(
  TARGET ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
  COMMAND
    ${CMAKE_COMMAND} -E echo "Adding CSharp byproducts."
  POST_BUILD
  BYPRODUCTS
    ${_files}
)

# Configure AssemblyInfo.cs
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs.in
  ${CSHARP_SOURCE_DIRECTORY}/AssemblyInfo.cs
  @ONLY
)

# Add managed wrapper
csharp_add_library(
    SimpleITKCSharpManaged
    ${CSHARP_SOURCE_DIRECTORY}/*.cs
)
add_dependencies(
  SimpleITKCSharpManaged
  ${SWIG_MODULE_SimpleITKCSharpNative_TARGET_NAME}
)

# Packaging for distribution
add_subdirectory(dist)

if(NOT CMAKE_PROJECT_NAME STREQUAL "SimpleITK" AND BUILD_TESTING)
  set(WRAP_CSHARP 1)

  add_subdirectory(
    "${SimpleITK_SOURCE_DIR}/Testing/Unit/CSharp"
    "${CMAKE_BINARY_DIR}/Testing/Unit"
  )

  if(BUILD_EXAMPLES)
    add_subdirectory(
      "${SimpleITK_SOURCE_DIR}/Examples"
      "${CMAKE_BINARY_DIR}/Examples"
    )
  endif()

  if(COMMAND ExternalData_Add_Target)
    ExternalData_Add_Target(SimpleITKData)
  endif()
endif()
