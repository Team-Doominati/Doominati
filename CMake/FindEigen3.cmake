set(EIGEN3_SEARCH_PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /usr/local
   /usr
   /sw
   /opt/local
   /opt/csw
   /opt
)

set(EIGEN3_PATH_SUFFIXES
   include/eigen3
   include/eigen
   include/Eigen
   include
)

find_path(EIGEN3_INCLUDE_DIRS
   NAMES signature_of_eigen3_matrix_library
   PATH_SUFFIXES ${EIGEN3_PATH_SUFFIXES}
   PATHS ${EIGEN3_SEARCH_PATHS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EIGEN3 REQUIRED_VARS EIGEN3_INCLUDE_DIRS)

set(EIGEN3_FOUND "NO")
if(EIGEN3_INCLUDE_DIRS)
   set(EIGEN3_FOUND "YES")
endif()

## EOF
