include("${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/sitkCMakeInit.cmake")
cmake_minimum_required(
  VERSION ${SITK_CMAKE_MINIMUM_REQUIRED_VERSION}...${SITK_CMAKE_POLICY_VERSION}
)

project(SimpleITK_Python)

include(../../CMake/sitkProjectLanguageCommon.cmake NO_POLICY_SCOPE)

find_package(
  Python
  REQUIRED
  COMPONENTS
    Interpreter
    Development.Module
  OPTIONAL_COMPONENTS
    Development.SABIModule
)

include_directories(
  ${SimpleITK_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Options
#
option(
  SimpleITK_PYTHON_FLATSTATICMETHOD
  "Enable object_method functions for static class members. \
This feature will be removed in the future. To disable requires SWIG>=4.1.0"
  ON
)
option(
  SimpleITK_PYTHON_THREADS
  "Enable threaded python usage by unlocking the GIL."
  ON
)
mark_as_advanced(SimpleITK_PYTHON_THREADS)
option(
  SimpleITK_PYTHON_EGG
  "Add building of python eggs to the dist target."
  OFF
)
mark_as_advanced(SimpleITK_PYTHON_EGG)
option(
  SimpleITK_PYTHON_WHEEL
  "Add building of python wheels to the dist target."
  ON
)
mark_as_advanced(SimpleITK_PYTHON_WHEEL)

if(
  CMAKE_VERSION
    VERSION_GREATER_EQUAL
    "3.26"
  AND
    SWIG_VERSION
      VERSION_GREATER_EQUAL
      "4.2.0"
  AND
    Python_VERSION
      VERSION_GREATER_EQUAL
      "3.11.0"
)
  set(_SimpleITK_PYTHON_USE_LIMITED_API_DEFAULT ON)
else()
  set(_SimpleITK_PYTHON_USE_LIMITED_API_DEFAULT OFF)
endif()
option(
  SimpleITK_PYTHON_USE_LIMITED_API
  "Use Python limited API, for minor version compatibility."
  ${_SimpleITK_PYTHON_USE_LIMITED_API_DEFAULT}
)
mark_as_advanced(SimpleITK_PYTHON_USE_LIMITED_API)

if(SimpleITK_PYTHON_USE_LIMITED_API)
  find_package(
    Python
    REQUIRED
    COMPONENTS
      Interpreter
      Development.SABIModule
  )
endif()

if(
  SimpleITK_PYTHON_USE_LIMITED_API
  AND
    NOT
      _SimpleITK_PYTHON_USE_LIMITED_API_DEFAULT
)
  message(WARNING "SWIG_VERSION (>=3.26): ${SWIG_VERSION}")
  message(WARNING "Python_VERSION (>=3.11.0): ${Python_VERSION}")
  message(WARNING "CMAKE_VERSION (>=3.26): ${CMAKE_VERSION}")
  message(
    WARNING
    "Development.SABIModule_FOUND: ${Development.SABIModule_FOUND}"
  )
  message(
    FATAL_ERROR
    "Python limited API is not supported for this Python version and SWIG version."
  )
  set(SimpleITK_PYTHON_USE_LIMITED_API OFF)
endif()

set_source_files_properties(
  SimpleITK.i
  PROPERTIES
    CPLUSPLUS
      ON
)

# Run swig
set(
  CMAKE_SWIG_FLAGS
  ${CMAKE_SWIG_GLOBAL_FLAGS}
  -features
  autodoc=1
  -keyword
)
if(SimpleITK_PYTHON_THREADS)
  set(
    CMAKE_SWIG_FLAGS
    ${CMAKE_SWIG_FLAGS}
    -threads
  )
endif()
if(
  SimpleITK_PYTHON_FLATSTATICMETHOD
  AND
    SWIG_VERSION
      VERSION_GREATER_EQUAL
      "4.1.0"
)
  set(
    CMAKE_SWIG_FLAGS
    ${CMAKE_SWIG_FLAGS}
    -flatstaticmethod
  )
endif()
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/SimpleITK)
set(
  SWIG_MODULE_SimpleITK_EXTRA_DEPS
  ${SWIG_EXTRA_DEPS}
  ${CMAKE_CURRENT_SOURCE_DIR}/Python.i
  ${CMAKE_CURRENT_SOURCE_DIR}/sitkImage.i
  ${CMAKE_CURRENT_SOURCE_DIR}/sitkTransform.i
  ${CMAKE_CURRENT_SOURCE_DIR}/sitkPathType.i
)
swig_add_module( SimpleITK python
  SimpleITK.i
  sitkPyCommand.cxx
)
set(
  SWIG_MODULE_SimpleITKPython_TARGET_NAME
  "${SWIG_MODULE_SimpleITK_TARGET_NAME}"
)
target_link_libraries(
  ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
  ${SimpleITK_LIBRARIES}
)
if(NOT Python_DEBUG_LIBRARY AND MSVC)
  # If there is not a specified debug library use the release library,
  # via a special sitkPython.h header.
  target_compile_definitions(
    ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
    PRIVATE
      $<$<CONFIG:Debug>:SWIG_PYTHON_INTERPRETER_NO_DEBUG>
  )
endif()

get_property(
  _suffix
  TARGET ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
  PROPERTY SUFFIX
)
if(NOT _suffix)
  set(_suffix "${CMAKE_SHARED_MODULE_SUFFIX}")
endif()
if(SimpleITK_PYTHON_USE_LIMITED_API)
  # https://docs.python.org/3/c-api/stable.html

  sitk_target_link_libraries_with_dynamic_lookup( ${SWIG_MODULE_SimpleITKPython_TARGET_NAME} Python::SABIModule)
  target_compile_definitions(
    ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
    PRIVATE
      -DPy_LIMITED_API=0x030b0000
  )

  message(
    STATUS
    "Setting limited ABI suffix for Python module to: '${Python_SOSABI}${_suffix}' libraries: ${Python_LIBRARIES}"
  )
  set_property(
    TARGET
      ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
    PROPERTY
      SUFFIX
        ".${Python_SOSABI}${_suffix}"
  )
else()
  sitk_target_link_libraries_with_dynamic_lookup( ${SWIG_MODULE_SimpleITKPython_TARGET_NAME} Python::Module)
  message(
    STATUS
    "Setting ABI suffix for Python module to: '${Python_SOABI}${_suffix}' "
  )
  set_property(
    TARGET
      ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
    PROPERTY
      SUFFIX
        ".${Python_SOABI}${_suffix}"
  )
endif()

target_include_directories(
  ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
  PRIVATE
    ${Python_INCLUDE_DIRS}
)
target_include_directories(
  ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(
  ${SWIG_MODULE_SimpleITK_TARGET_NAME}
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
      "${CMAKE_SWIG_OUTDIR}"
)
if(MSVC)
  foreach(CMAKE_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CMAKE_CONFIGURATION_TYPE} CMAKE_CONFIGURATION_TYPE)
    set_target_properties(
      ${SWIG_MODULE_SimpleITK_TARGET_NAME}
      PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE}
          "${CMAKE_SWIG_OUTDIR}"
    )
    set_target_properties(
      ${SWIG_MODULE_SimpleITK_TARGET_NAME}
      PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE}
          "${CMAKE_SWIG_OUTDIR}"
    )
  endforeach()
endif()

# TODO add check to determine if compiler support linker script
if(UNIX AND NOT APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(VERSION_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/symbols.map")
  set_target_properties(
    ${SWIG_MODULE_SimpleITK_TARGET_NAME}
    PROPERTIES
      LINK_DEPENDS
        "${VERSION_SCRIPT}"
  )
  set_property(
    TARGET
      ${SWIG_MODULE_SimpleITK_TARGET_NAME}
    APPEND_STRING
    PROPERTY
      LINK_FLAGS
        " -Wl,--version-script=${VERSION_SCRIPT}"
  )
endif()

target_compile_options(${SWIG_MODULE_SimpleITKPython_TARGET_NAME} PRIVATE -w)
sitk_strip_target( ${SWIG_MODULE_SimpleITKPython_TARGET_NAME} )

# Installation
set(SimpleITK_PYTHON_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
file(
  TO_NATIVE_PATH
  "${SimpleITK_PYTHON_PACKAGE_DIR}"
  SimpleITK_PYTHON_PACKAGE_DIR
)

set(
  SimpleITK_PYTHON_TEST_EXECUTABLE
  "${Python_EXECUTABLE}"
  CACHE INTERNAL
  "Python executable for testing."
  FORCE
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/SimpleITK/_version.py.in"
  "${CMAKE_CURRENT_BINARY_DIR}/SimpleITK/_version.py"
)

set(
  SimpleITK_Py_Files
  "__init__.py"
  "extra.py"
  "py.typed"
)

if(DEFINED SKBUILD)
  # Currently this installation
  install(
    FILES
      ${CMAKE_SWIG_OUTDIR}/SimpleITK.py
      ${SimpleITK_DOC_FILES}
      "${CMAKE_CURRENT_BINARY_DIR}/SimpleITK/_version.py"
    DESTINATION SimpleITK
    COMPONENT Runtime
  )

  foreach(_file ${SimpleITK_Py_Files})
    install(
      FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/SimpleITK/${_file}"
      DESTINATION SimpleITK
      COMPONENT Runtime
    )
  endforeach()

  install(
    TARGETS
      ${SWIG_MODULE_SimpleITKPython_TARGET_NAME}
    RUNTIME
      DESTINATION SimpleITK
      COMPONENT Runtime
    LIBRARY
      DESTINATION SimpleITK
      COMPONENT Runtime
  )
else()
  include(LegacyPackaging.cmake)
endif()

if(NOT CMAKE_PROJECT_NAME STREQUAL "SimpleITK" AND BUILD_TESTING)
  set(WRAP_PYTHON 1)

  add_subdirectory(
    "${SimpleITK_SOURCE_DIR}/Testing/Unit/Python"
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
