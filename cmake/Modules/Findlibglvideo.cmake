# - Try to find libglvideo
# Once done this will define
#  LIBGLVIDEO_FOUND - System has libglvideo
#  LIBGLVIDEO_INCLUDE_DIRS - The libglvideo include directories
#  LIBGLVIDEO_LIBRARIES - The libraries needed to use libglvideo

set(LIBGLVIDEO_LIBRARY libglvideo)
get_filename_component(LIBGLVIDEO_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

include("${LIBGLVIDEO_ROOT_DIR}/cmake/libglvideoConfig.cmake")

#set(LIBGLVIDEO_INCLUDE_DIR "${LIBGLVIDEO_DIR}/include")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBGLVIDEO_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBGLVIDEO DEFAULT_MSG
                                  LIBGLVIDEO_LIBRARY LIBGLVIDEO_INCLUDE_DIRS)

mark_as_advanced(LIBGLVIDEO_INCLUDE_DIR LIBGLVIDEO_LIBRARY)

set(LIBGLVIDEO_LIBRARIES ${LIBGLVIDEO_LIBRARY} )
#set(LIBGLVIDEO_INCLUDE_DIRS ${LIBGLVIDEO_INCLUDE_DIR} )
