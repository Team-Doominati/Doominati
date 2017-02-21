find_path(EIGEN3_INCLUDE_DIRS
   signature_of_eigen3_matrix_library
   PATH_SUFFIXES eigen3 eigen Eigen)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EIGEN3 REQUIRED_VARS EIGEN3_INCLUDE_DIRS)

## EOF
