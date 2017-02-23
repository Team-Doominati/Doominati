##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Find package for Eigen 3.
##
## A math library for C++ with many optimizations.
##
##-----------------------------------------------------------------------------

find_path(EIGEN3_INCLUDE_DIRS
   signature_of_eigen3_matrix_library
   PATH_SUFFIXES eigen3 eigen Eigen)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Eigen3 REQUIRED_VARS EIGEN3_INCLUDE_DIRS)

## EOF
