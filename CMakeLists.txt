#
# A simple low-level Lua wrapper for pthreads.
#
cmake_minimum_required(VERSION 2.8)

project(lua-llthreads C)

set(BUILD_SHARED_LIBS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(INSTALL_CMOD ${CMAKE_INSTALL_PREFIX}/lib/lua/5.1 CACHE PATH
				"Directory to install Lua binary modules (configure lua via LUA_CPATH)")

set(COMMON_CFLAGS "${CFLAGS}")
set(COMMON_LDFLAGS)
set(COMMON_LIBS)

## Lua 5.1.x
include(FindLua51)
if(NOT ${LUA51_FOUND})
    message(FATAL_ERROR "The FindLua51 module could not find lua :-(")
endif()
if(WIN32)
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -I${LUA_INCLUDE_DIR}")
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${LUA_LIBRARY}")
endif()
## MAC OSX needs extra linker flags
if(APPLE)
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} -undefined dynamic_lookup")
endif()

## find threads
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
include(FindThreads)

include(CustomMacros)

## LuaNativeObjects
include(LuaNativeObjects)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${LUA_INCLUDE_DIR})

## LuaLLTHREADS
set(LUA_LLTHREADS_SRC
	llthreads.nobj.lua
)

if(${USE_PRE_GENERATED_BINDINGS})
	set(LUA_LLTHREADS_SRC src/pre_generated-llthreads.nobj.c)
else()
	# Generate Lua bindings.
	GenLuaNativeObjects(LUA_LLTHREADS_SRC)
endif()

add_library(lua-llthreads MODULE ${LUA_LLTHREADS_SRC})
target_link_libraries(lua-llthreads ${COMMON_LIBS} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(lua-llthreads PROPERTIES PREFIX "")
add_target_properties(lua-llthreads COMPILE_FLAGS "${COMMON_CFLAGS}")
add_target_properties(lua-llthreads LINK_FLAGS "${LD_FLAGS} ${COMMON_LDFLAGS}")
set_target_properties(lua-llthreads PROPERTIES OUTPUT_NAME llthreads)

install(TARGETS lua-llthreads
        DESTINATION "${INSTALL_CMOD}")

