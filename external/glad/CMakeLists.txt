# GLAD: OpenGL 3.3 Core loader generated with https://glad.dav1d.de
#   Language: C/C++   Specification: OpenGL   API gl: 3.3   Profile: Core
add_library(glad STATIC
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# glad.c uses dlopen() on Linux to find the OpenGL library
if(UNIX AND NOT APPLE)
    target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})
endif()
