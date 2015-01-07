add_executable(thread_server ThreadServer.cc)
target_link_libraries(thread_server muduo_net)

#add_library(base_server ${base_server_SRCS})
#target_link_libraries(base_server muduo_net)
