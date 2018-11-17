cmake_minimum_required(VERSION 3.4.1)
add_library(jniwrap
             SHARED
             src/main/cpp/main.cc
              )
target_link_libraries(jniwrap log)
