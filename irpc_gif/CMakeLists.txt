#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_C_FLAGS "-std=c++11")
project(GIF++)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
option(G4ANALYSIS_USE "use ROOT" ON)
#SET(CMAKE_MODULE_PATH /afs/cern.ch/sw/lcg/external/geant4/10.1/share/cmake/Modules)
#SET(G4NEUTRONHP_USE_ONLY_PHOTONEVAPORATION ON)
set(CMAKE_BUILD_TYPE "Debug")
 

find_package(ROOT)

if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


add_definitions(-DG4VERBOSE -DG4UI_USE -DG4VIS_USE -DG4ANALYSIS_USE)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories($ENV{ROOTSYS}/include)


#----------------------------------------------------------------------------
# Find ROOT (required package)
#
#find_package(ROOT QUIET)
#add_definitions(-DG4ANALYSIS_USE_ROOT)
#if(NOT ROOT_FOUND)
#   message(STATUS "G4 Examples: ROOT package not found. --> 3GEM-G4 example #disabled")  
#  return()
#endif()

link_directories($ENV{ROOTSYS}/lib)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(GIF++ GIF++.cc ${sources} ${headers})


#if(ROOT_FOUND)
   target_link_libraries(GIF++ ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
#else()
 #  target_link_libraries(GIF++ ${Geant4_LIBRARIES})
#endif()






#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build iRPC. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEiRPC_SCRIPTS
   /macros/GIFpp_DS_US.gdml
   /macros/vis.mac
   /macros/icons.mac
   /macros/run.mac
)

foreach(_script ${EXAMPLEiRPC_SCRIPTS})
   configure_file(
      ${PROJECT_SOURCE_DIR}/${_script}
      ${PROJECT_BINARY_DIR}/${_script}
      COPYONLY
      )
endforeach()



#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS GIF++ DESTINATION bin )
