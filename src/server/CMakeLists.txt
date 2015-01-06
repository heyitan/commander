add_subdirectory(base_server)


#if(CARES_INCLUDE_DIR AND CARES_LIBRARY)
#  add_subdirectory(cdns)
#else()
#  add_subdirectory(cdns EXCLUDE_FROM_ALL)
#endif()
#
#if(CURL_FOUND)
#  add_subdirectory(curl)
#else()
#  add_subdirectory(curl EXCLUDE_FROM_ALL)
#endif()
#
#if(PROTOBUF_FOUND)
#  add_subdirectory(ace/logging)
#  add_subdirectory(protobuf)
#else()
#  add_subdirectory(ace/logging EXCLUDE_FROM_ALL)
#  add_subdirectory(protobuf EXCLUDE_FROM_ALL)
#endif()
#
#if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
#  add_subdirectory(hiredis)
#else()
#  add_subdirectory(hiredis EXCLUDE_FROM_ALL)
#endif()
#
#if(GD_INCLUDE_DIR AND GD_LIBRARY)
#  add_subdirectory(procmon)
#else()
#  add_subdirectory(procmon EXCLUDE_FROM_ALL)
#endif()
