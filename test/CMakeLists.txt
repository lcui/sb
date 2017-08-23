cmake_minimum_required (VERSION 2.8)


project (sb)

#add_subdirectory(tests)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DHAVE_CONFIG_H=1)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D__MSC__)
    add_definitions(-D_USE_MSC_)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} "-framework QuartzCore -framework AppKit -framework OpenGL -framework QTKit -framework Foundation -framework CoreVideo -framework AVFoundation -framework CoreMedia -framework CoreFoundation -framework VideoToolbox -framework AudioToolbox -framework VideoDecodeAcceleration -Wl,-framework,CoreFoundation -Wl,-framework,Security -framework CoreGraphics -lm -llzma -framework CoreServices")
    set(OTHER_LIBS 
        )
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message ("Unix")
    add_definitions(-D_USE_LINUX_)
    set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11 ) # add c++11 flag
    set(OTHER_LIBS 
        z
        m
        )
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories(
    ../sphinxbase/include
    ../sphinxbase/include/win32
    ../pocketsphinx/include
    )

add_definitions(-D__STDC_CONSTANT_MACROS)

set(SPHINX_LIB
    sphinxbase
    pocketsphinx
    )

set (SOURCES_PLOT
    test.cpp
    )

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../pocketsphinx/bin/Debug/x64
    )
add_executable(sb
    ${SOURCES_PLOT}
    )
target_link_libraries(sb
    ${SPHINX_LIB}
    ${OTHER_LIBS}
    )

