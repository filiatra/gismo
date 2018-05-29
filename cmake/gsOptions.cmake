######################################################################
## CMakeLists.txt ---
## This file is part of the G+Smo library. 
##
## Author: Angelos Mantzaflaris 
## Copyright (C) 2012 - 2016 RICAM-Linz.
######################################################################

#macro(print_gismo_option NAME)
##option(${NAME} ${DESCR} false)
#if (${NAME})
#  message ("  ${NAME}        ${${NAME}}")
#endif()
#endmacro(print_gismo_option)

## #################################################################
## Options list
## #################################################################

#Standard options
option(GISMO_EXTRA_DEBUG         "Extra debug features"      false  )
option(GISMO_BUILD_LIB           "Build shared library"      true   )
option(GISMO_BUILD_PCH           "Build precompiled headers" false  )
option(GISMO_BUILD_EXAMPLES      "Build examples"            true   )
option(GISMO_BUILD_UNITTESTS     "Build unittests"           false  )
option(GISMO_BUILD_AXL           "Build Axel Plugin"         false  )
option(GISMO_BUILD_RHINOPLUGINS  "Build Rhino THB Plugins"   false  )
option(GISMO_BUILD_PVIEW         "Build Paraview Plugin"     false  )
option(GISMO_BUILD_MEX           "Build Mex files"           false  )
option(GISMO_WITH_OPENMP         "With OpenMP"               false  )
option(GISMO_WITH_MPI            "With MPI"                  false  )
option(GISMO_WITH_PSOLID         "With Parasolid"            false  )
option(GISMO_WITH_ONURBS         "With OpenNurbs"            false  )
option(GISMO_WITH_FDBB           "With FDBB"                 false  )
option(GISMO_WITH_IPOPT          "With IpOpt"                false  )
option(GISMO_WITH_ADIFF          "With auto-diff"            false  )
option(GISMO_WITH_CODIPACK       "With CoDiPack"             false  )
option(GISMO_WITH_SUPERLU        "With SuperLU"              false  )
option(GISMO_WITH_TAUCS          "With Taucs"                false  )
option(GISMO_WITH_UMFPACK        "With Umfpack"              false  )
option(GISMO_WITH_PARDISO        "With PARDISO"              false  )
#option(GISMO_WITH_METIS          "With METIS"                false )
option(GISMO_WITH_TRILINOS       "With TRILINOS"             false  )
option(GISMO_WITH_PASTIX         "With PastiX"               false  )
option(GISMO_WITH_SPECTRA        "With Spectra"              false  )
option(GISMO_WITH_UNUM           "With Universal NUMber"     false  )

#Extra options
option(GISMO_BUILD_QT_APP        "Build Qt application"          false  )
option(GISMO_WARNINGS            "Enable G+Smo related warnings" false)
#option(GISMO_WITH_VTK            "With VTK"                      false  )
option(GISMO_BUILD_CPPLOT        "Build cpplot"                  false  )
option(EIGEN_USE_MKL_ALL         "Eigen use MKL"                 false  )
if(CMAKE_COMPILER_IS_GNUCXX)
 option(GISMO_BUILD_COVERAGE      "Build with coverage"          false )
endif(CMAKE_COMPILER_IS_GNUCXX)

message ("Configuration (cmake ${CMAKE_VERSION}):")
message ("  Source folder:          ${CMAKE_SOURCE_DIR}")
message ("  CMAKE_BUILD_TYPE        ${CMAKE_BUILD_TYPE}")
message ("  GISMO_COEFF_TYPE        ${GISMO_COEFF_TYPE}")
message ("  GISMO_BUILD_LIB         ${GISMO_BUILD_LIB}")
message ("  CMAKE_C_COMPILER        ${CMAKE_C_COMPILER}")
message ("  CMAKE_CXX_COMPILER      ${CMAKE_CXX_COMPILER}")
message ("  CMAKE_CXX_STANDARD      ${CMAKE_CXX_STANDARD}")

if (${GISMO_BUILD_PCH})
message ("  GISMO_BUILD_PCH         ${GISMO_BUILD_PCH}")
endif()
if (${GISMO_EXTRA_DEBUG})
message ("  GISMO_EXTRA_DEBUG       ${GISMO_EXTRA_DEBUG}")
endif()
if (${GISMO_BUILD_EXAMPLES})
message ("  GISMO_BUILD_EXAMPLES    ${GISMO_BUILD_EXAMPLES}")
endif()
if (${GISMO_BUILD_UNITTESTS})
message ("  GISMO_BUILD_UNITTESTS   ${GISMO_BUILD_UNITTESTS}")
endif()
if (${GISMO_BUILD_AXL})
message ("  GISMO_BUILD_AXL         ${GISMO_BUILD_AXL}")
endif()
#message ("  GISMO_BUILD_PVIEW       ${GISMO_BUILD_PVIEW}")
#message ("  GISMO_BUILD_MEX         ${GISMO_BUILD_MEX}")
#message ("  GISMO_WITH_OPENMP       ${GISMO_WITH_OPENMP}")
if (${GISMO_WITH_PSOLID})
message ("  GISMO_WITH_PSOLID       ${GISMO_WITH_PSOLID}")
endif()
#message ("  GISMO_WITH_MPFR         ${GISMO_WITH_MPFR}")
if (${GISMO_WITH_ONURBS})
message ("  GISMO_WITH_ONURBS       ${GISMO_WITH_ONURBS}")
endif()
if (${GISMO_WITH_FDBB})
message ("  GISMO_WITH_FDBB         ${GISMO_WITH_FDBB}")
endif()
if (${GISMO_WITH_IPOPT})
message ("  GISMO_WITH_IPOPT        ${GISMO_WITH_IPOPT}")
endif()
if (${GISMO_WITH_SUPERLU})
message ("  GISMO_WITH_SUPERLU      ${GISMO_WITH_SUPERLU}")
endif()
if (${GISMO_WITH_TAUCS})
message ("  GISMO_WITH_TAUCS        ${GISMO_WITH_TAUCS}")
endif()
if (${GISMO_WITH_UMFPACK})
message ("  GISMO_WITH_UMFPACK      ${GISMO_WITH_UMFPACK}")
endif()
if (${GISMO_WITH_PARDISO})
message ("  GISMO_WITH_PARDISO      ${GISMO_WITH_PARDISO}")
endif()
if (${GISMO_WITH_TRILINOS})
message ("  GISMO_WITH_TRILINOS     ${GISMO_WITH_TRILINOS}")
endif()
if (${GISMO_WITH_OPENMP})
message ("  GISMO_WITH_OPENMP       ${GISMO_WITH_OPENMP}")
endif()
if (${GISMO_WITH_MPI})
message ("  GISMO_WITH_MPI          ${GISMO_WITH_MPI}")
endif()
if (${GISMO_WITH_ADIFF})
message ("  GISMO_WITH_ADIFF        ${GISMO_WITH_ADIFF}")
endif()
if (${GISMO_WITH_CODIPACK})
message ("  GISMO_WITH_CODIPACK     ${GISMO_WITH_CODIPACK}")
endif()
if (${GISMO_WITH_SPECTRA})
message ("  GISMO_WITH_SPECTRA      ${GISMO_WITH_SPECTRA}")
endif()
if (${GISMO_WITH_UNUM})
message ("  GISMO_WITH_UNUM         ${GISMO_WITH_UNUM}")
endif()
if (EIGEN_USE_MKL_ALL)
message ("  EIGEN_USE_MKL_ALL       ${EIGEN_USE_MKL_ALL}")
endif()

#https://www.threadingbuildingblocks.org/documentation
#message ("  GISMO_WITH_ITBB          ${GISMO_WITH_ITBB}")


#message(STATUS "Type cmake -LAH to see all variables")
