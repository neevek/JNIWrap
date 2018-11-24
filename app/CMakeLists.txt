cmake_minimum_required(VERSION 3.4.1)
add_library(jniwrap
        SHARED
        src/main/cpp/main.cc
        src/main/cpp/jniwrap/jni_env_wrap.cc
        )
include_directories(
        src/main/cpp
)
target_link_libraries(jniwrap log)
