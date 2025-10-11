cmake_minimum_required(VERSION 3.18)
project(sopagrami LANGUAGES CXX)

# ---- global settings -------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)   # needed for linking into Python .so

option(SOPAGRAMI_BUILD_CLI     "Build C++ CLI executables" ON)
option(SOPAGRAMI_BUILD_PYTHON  "Build Python extension module" OFF)
option(SOPAGRAMI_USE_OPENMP    "Enable OpenMP if available" ON)

# ---- dependencies ----------------------------------------------------
if (SOPAGRAMI_USE_OPENMP)
  find_package(OpenMP)
  if (OpenMP_CXX_FOUND)
    add_compile_options(${OpenMP_CXX_FLAGS})
    add_link_options(${OpenMP_CXX_FLAGS})
  endif()
endif()

# pybind11 is optional – only find it when building the Python module
if (SOPAGRAMI_BUILD_PYTHON)
  find_package(pybind11 REQUIRED)   # scikit-build will provide this
endif()

# ---- core library ----------------------------------------------------
add_library(alg STATIC
  src/alg.cpp
)
set_target_properties(alg PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(alg PUBLIC include)
if (OpenMP_CXX_FOUND)
  target_link_libraries(alg PUBLIC OpenMP::OpenMP_CXX)
endif()

# ---- CLI executables (optional) --------------------------------------
if (SOPAGRAMI_BUILD_CLI)
  add_executable(run src/main.cpp)
  target_link_libraries(run PRIVATE alg)
  target_include_directories(run PRIVATE include)
endif()

# ---- Python extension (optional) -------------------------------------
if (SOPAGRAMI_BUILD_PYTHON)
  # Name must match your Python import: sopagrami/_core.*
  pybind11_add_module(_core MODULE src/bindings.cpp)
  target_link_libraries(_core PRIVATE alg)
  target_include_directories(_core PRIVATE include)
  set_target_properties(alg PROPERTIES POSITION_INDEPENDENT_CODE ON)
  if (OpenMP_CXX_FOUND)
    target_link_libraries(_core PRIVATE OpenMP::OpenMP_CXX)
  endif()

  # When installing via CMake, drop the module into a folder named 'sopagrami'
  install(TARGETS _core
    LIBRARY DESTINATION sopagrami
    RUNTIME DESTINATION sopagrami
    ARCHIVE DESTINATION sopagrami
  )
endif()
