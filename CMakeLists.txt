#
# A simple low-level Lua wrapper for pthreads.
#
cmake_minimum_required(VERSION 2.8)

project(lua-llthreads C)

set(BUILD_SHARED_LIBS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(INSTALL_CMOD share/lua/cmod CACHE PATH "Directory to install Lua binary modules (configure lua via LUA_CPATH)")
set(LUA_NATIVE_OBJECTS_PATH ../LuaNativeObjects CACHE PATH
				"Directory to LuaNativeObjects bindings generator.")
set(USE_PRE_GENERATED_BINDINGS TRUE CACHE BOOL
				"Set this to FALSE to re-generate bindings using LuaNativeObjects")

set(COMMON_CFLAGS "${CFLAGS}")
set(COMMON_LDFLAGS)
set(COMMON_LIBS)

## Lua 5.1.x
include(FindLua51)
if(NOT ${LUA51_FOUND})
    message(FATAL_ERROR "The FindLua51 module could not find lua :-(")
endif()
set(COMMON_LIBS "${COMMON_LIBS};${LUA_LIBRARIES}")

## find threads
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
include(FindThreads)

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
set_target_properties(lua-llthreads PROPERTIES COMPILE_FLAGS "${COMMON_CFLAGS}")
set_target_properties(lua-llthreads PROPERTIES OUTPUT_NAME llthreads)

install(TARGETS lua-llthreads
        DESTINATION "${INSTALL_CMOD}")

