set(base_server_SRCS
  server_threaded.cc
)


add_library(base_server ${base_server_SRCS})
target_link_libraries(base_server muduo_net)