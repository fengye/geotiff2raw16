cmake_minimum_required (VERSION 2.8.11)
project (geotiff2raw16)

add_executable(gtiff2r16 gtiff2r16.c)
add_executable(stitchr16 stitchr16.c)
add_executable(cutr16 cutr16.c)

# GeoTiff headers
find_path (GEOTIFF_INC NAMES libgeotiff geotiff PATHS /usr/include REQUIRED)
if (NOT GEOTIFF_INC)
  message(FATAL_ERROR "libgeotiff headers cannot be found.")
endif (NOT GEOTIFF_INC)
# GeoTiff library
find_library (GEOTIFF_LIB NAMES libgeotiff geotiff PATHS /usr/lib REQUIRED)
if (NOT GEOTIFF_LIB)
  message(FATAL_ERROR "libgeotiff libraries cannot be found.")
endif (NOT GEOTIFF_LIB)

# libtiff library
find_library (TIFF_LIB NAMES libtiff tiff PATHS /usr/lib REQUIRED)
if (NOT TIFF_LIB)
  message(FATAL_ERROR "libtiff libraries cannot be found.")
endif (NOT TIFF_LIB)

message(STATUS "libgeotiff header found at: " ${GEOTIFF_INC})
message(STATUS "libgeotiff library found at: " ${GEOTIFF_LIB})
message(STATUS "libtiff library found at: " ${TIFF_LIB})
include_directories(${GEOTIFF_INC})
target_link_libraries (gtiff2r16 ${GEOTIFF_LIB} ${TIFF_LIB})
