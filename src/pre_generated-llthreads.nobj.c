/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"



#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 1
#define LUAJIT_FFI 0
#define USE_FIELD_GET_SET_METHODS 0



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 1
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

#ifndef obj_type_free
#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	free((obj)); \
} while(0)
#endif

#ifndef obj_type_new
#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = malloc(sizeof(type)); \
} while(0)
#endif

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef struct reg_sub_module {
	obj_type        *type;
	int             is_package;
	const luaL_reg  *pub_funcs;
	const luaL_reg  *methods;
	const luaL_reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char *obj_udata_weak_ref_key = "obj_udata_weak_ref_key";

#if LUAJIT_FFI
typedef struct ffi_export_symbol {
	const char *name;
	void       *sym;
} ffi_export_symbol;
#endif




static obj_type obj_types[] = {
#define obj_type_id_Lua_LLThread 0
#define obj_type_Lua_LLThread (obj_types[obj_type_id_Lua_LLThread])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "Lua_LLThread" },
  {NULL, -1, 0, NULL},
};


#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_objlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					luaL_error(L, "null %s", type->name); /* object was garbage collected? */
				}
				return ud;
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type);
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type);
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	*flags = OBJ_UDATA_FLAG_OWN;
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	/* create new userdata. */
	void *ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_objlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_objlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_register(L, NULL, reg_list);
	}

	obj_type_register_constants(L, type_reg->constants, -1);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->is_package == 1) {
		obj_type_register_package(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		luaL_register(L, NULL, reg_list); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	luaL_register(L, NULL, type_reg->methods); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

#if LUAJIT_FFI
	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */
#else
	(void)priv_table;
#endif

	luaL_register(L, NULL, type_reg->metas); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

#if LUAJIT_FFI
static int nobj_udata_new_ffi(lua_State *L) {
	size_t size = luaL_checkinteger(L, 1);
	void *ud;
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_settop(L, 2);
	/* create userdata. */
	ud = lua_newuserdata(L, size);
	lua_replace(L, 1);
	/* set userdata's metatable. */
	lua_setmetatable(L, 1);
	return 1;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code, const ffi_export_symbol *ffi_exports, int priv_table)
{
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = luaL_loadbuffer(L, ffi_init_code, strlen(ffi_init_code), ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushcfunction(L, nobj_udata_new_ffi);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


#define obj_type_Lua_LLThread_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Lua_LLThread))
#define obj_type_Lua_LLThread_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Lua_LLThread), flags)
#define obj_type_Lua_LLThread_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Lua_LLThread), flags)






#ifdef __WINDOWS__
#include <windows.h>
#include <stdio.h>
#include <process.h>
#else
#include <pthread.h>
#include <stdio.h>
#endif

typedef enum {
	TSTATE_NONE     = 0,
	TSTATE_STARTED  = 1<<0,
	TSTATE_DETACHED = 1<<1,
	TSTATE_JOINED   = 1<<2,
} Lua_TState;

typedef struct Lua_LLThread_child {
	lua_State  *L;
	int        status;
	int        is_detached;
} Lua_LLThread_child;

typedef struct Lua_LLThread {
	Lua_LLThread_child *child;
#ifdef __WINDOWS__
	HANDLE     thread;
#else
	pthread_t  thread;
#endif
	Lua_TState state;
} Lua_LLThread;

#define ERROR_LEN 1024

/******************************************************************************
* traceback() function from Lua 5.1.x source.
* Copyright (C) 1994-2008 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
static int traceback (lua_State *L) {
  if (!lua_isstring(L, 1))  /* 'message' not a string? */
    return 1;  /* keep it intact */
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);  /* pass error message */
  lua_pushinteger(L, 2);  /* skip this function and traceback */
  lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}

static Lua_LLThread_child *llthread_child_new() {
	Lua_LLThread_child *this;

	this = (Lua_LLThread_child *)calloc(1, sizeof(Lua_LLThread_child));
	/* create new lua_State for the thread. */
	this->L = luaL_newstate();
	/* open standard libraries. */
	luaL_openlibs(this->L);
	/* push traceback function as first value on stack. */
	lua_pushcfunction(this->L, traceback);

	return this;
}

static void llthread_child_destroy(Lua_LLThread_child *this) {
	lua_close(this->L);
	free(this);
}

static Lua_LLThread *llthread_new() {
	Lua_LLThread *this;

	this = (Lua_LLThread *)calloc(1, sizeof(Lua_LLThread));
	this->state = TSTATE_NONE;
	this->child = llthread_child_new();

	return this;
}

static void llthread_destroy(Lua_LLThread *this) {
	/* We still own the child thread object iff the thread was not started or
	 * we have joined the thread.
	 */
	if((this->state & TSTATE_JOINED) == TSTATE_JOINED || this->state == TSTATE_NONE) {
		if(this->child) llthread_child_destroy(this->child);
		this->child = NULL;
	}
	free(this);
}

#ifdef __WINDOWS__
static void run_child_thread(void *arg) {
#else
static void *run_child_thread(void *arg) {
#endif
	Lua_LLThread_child *this = (Lua_LLThread_child *)arg;
	lua_State *L = this->L;
	int nargs = lua_gettop(L) - 2;

	this->status = lua_pcall(L, nargs, LUA_MULTRET, 1);

	/* alwasy print errors here, helps with debugging bad code. */
	if(this->status != 0) {
		const char *err_msg = lua_tostring(L, -1);
		fprintf(stderr, "Error from thread: %s\n", err_msg);
		fflush(stderr);
	}

	/* if thread is detached, then destroy the child state. */
	if(this->is_detached != 0) {
		/* thread is detached, so it must clean-up the child state. */
		llthread_child_destroy(this);
		this = NULL;
	}
#ifdef __WINDOWS__
	if(this) {
		/* attached thread, don't close thread handle. */
		_endthreadex(0);
	} else {
		/* detached thread, close thread handle. */
		_endthread();
	}
#else
	return this;
#endif
}

static int llthread_start(Lua_LLThread *this, int start_detached) {
	Lua_LLThread_child *child;
	int rc = 0;

	child = this->child;
	child->is_detached = start_detached;
#ifdef __WINDOWS__
	this->thread = (HANDLE)_beginthread(run_child_thread, 0, child);
	if(this->thread != (HANDLE)-1L) {
		this->state = TSTATE_STARTED;
		if(start_detached) {
			this->state |= TSTATE_DETACHED;
		}
	}
#else
	rc = pthread_create(&(this->thread), NULL, run_child_thread, child);
	if(rc == 0) {
		this->state = TSTATE_STARTED;
		if(start_detached) {
			this->state |= TSTATE_DETACHED;
			rc = pthread_detach(this->thread);
		}
	}
#endif
	return rc;
}

static int llthread_join(Lua_LLThread *this) {
#ifdef __WINDOWS__
	WaitForSingleObject( this->thread, INFINITE );
	/* Destroy the thread object. */
	CloseHandle( this->thread );

	return 0;
#else
	Lua_LLThread_child *child;
	int rc;

	/* then join the thread. */
	rc = pthread_join(this->thread, (void **)&(child));
	if(rc == 0) {
		this->state |= TSTATE_JOINED;
		/* if the child thread returns NULL, then it freed the child object. */
		this->child = child;
	}
	return rc;
#endif
}

static int llthread_move_values(lua_State *from_L, lua_State *to_L, int idx, int top, int is_arg) {
	const char *str;
	size_t str_len;
	int nvalues = 0;
	int n;
	int key, value;

	for(n = idx; n <= top; n++) {
		/* only support string/number/boolean/nil/lightuserdata. */
		switch(lua_type(from_L, n)) {
		case LUA_TNIL:
			lua_pushnil(to_L);
			break;
		case LUA_TNUMBER:
			lua_pushnumber(to_L, lua_tonumber(from_L, n));
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(to_L, lua_toboolean(from_L, n));
			break;
		case LUA_TSTRING:
			str = lua_tolstring(from_L, n, &(str_len));
			lua_pushlstring(to_L, str, str_len);
			break;
		case LUA_TLIGHTUSERDATA:
			lua_pushlightuserdata(to_L, lua_touserdata(from_L, n));
			break;
		case LUA_TTABLE:
			lua_newtable(to_L);
			lua_pushnil(from_L);
			while (lua_next(from_L, n) != 0) {
				/* key is at -2, value at -1, but we need to normalize these
				 * to positive indices */
				key = lua_gettop(from_L) - 1;
				value = lua_gettop(from_L);
				llthread_move_values(from_L, to_L, key, value, is_arg);
				/* Copied key and value are now at -2 and -1 in to_L. */
				lua_settable(to_L, -3);
				/* Pop value for next iteration */
				lua_pop(from_L, 1);
			}
			break;
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		default:
			if (is_arg) {
				return luaL_argerror(from_L, n, "function/userdata/thread types un-supported.");
			} else {
				/* convert un-supported types to an error string. */
				lua_pushfstring(to_L, "Un-supported value: %s: %p",
					lua_typename(from_L, lua_type(from_L, n)), lua_topointer(from_L, n));
			}
		}
		++nvalues;
	}

	return nvalues;
}

static int llthread_push_args(lua_State *L, Lua_LLThread_child *child, int idx, int top) {
	return llthread_move_values(L, child->L, idx, top, 1 /* is_arg */);
}

static int llthread_push_results(lua_State *L, Lua_LLThread_child *child, int idx, int top) {
	return llthread_move_values(child->L, L, idx, top, 0 /* is_arg */);
}

static Lua_LLThread *llthread_create(lua_State *L, const char *code, size_t code_len) {
	Lua_LLThread *this;
	Lua_LLThread_child *child;
	const char *str;
	size_t str_len;
	int rc;
	int top;

	this = llthread_new();
	child = this->child;
	/* load Lua code into child state. */
	rc = luaL_loadbuffer(child->L, code, code_len, code);
	if(rc != 0) {
		/* copy error message to parent state. */
		str = lua_tolstring(child->L, -1, &(str_len));
		if(str != NULL) {
			lua_pushlstring(L, str, str_len);
		} else {
			/* non-string error message. */
			lua_pushfstring(L, "luaL_loadbuffer() failed to load Lua code: rc=%d", rc);
		}
		llthread_destroy(this);
		lua_error(L);
		return NULL;
	}
	/* copy extra args from main state to child state. */
	top = lua_gettop(L);
	/* Push all args after the Lua code. */
	llthread_push_args(L, child, 2, top);

	return this;
}




/* method: delete */
static int Lua_LLThread__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Lua_LLThread * this_idx1 = obj_type_Lua_LLThread_delete(L,1,&(this_flags_idx1));
	Lua_LLThread_child *child;

  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	/* if the thread has been started and has not been detached/joined. */
	if((this_idx1->state & TSTATE_STARTED) == TSTATE_STARTED &&
			(this_idx1->state & (TSTATE_DETACHED|TSTATE_JOINED)) == 0) {
		/* then join the thread. */
		llthread_join(this_idx1);
		child = this_idx1->child;
		if(child && child->status != 0) {
			const char *err_msg = lua_tostring(child->L, -1);
			fprintf(stderr, "Error from non-joined thread: %s\n", err_msg);
			fflush(stderr);
		}
	}
	llthread_destroy(this_idx1);

  return 0;
}

/* method: start */
static int Lua_LLThread__start__meth(lua_State *L) {
  Lua_LLThread * this_idx1 = obj_type_Lua_LLThread_check(L,1);
  bool start_detached_idx2 = lua_toboolean(L,2);
  bool res_idx1 = 0;
	char buf[ERROR_LEN];
	int rc;

	if(this_idx1->state != TSTATE_NONE) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Thread already started.");
		return 2;
	}
	if((rc = llthread_start(this_idx1, start_detached_idx2)) != 0) {
		lua_pushboolean(L, 0); /* false */
		strerror_r(errno, buf, ERROR_LEN);
		lua_pushstring(L, buf);
		return 2;
	}
	res_idx1 = true;

  lua_pushboolean(L, res_idx1);
  return 1;
}

/* method: join */
static int Lua_LLThread__join__meth(lua_State *L) {
  Lua_LLThread * this_idx1 = obj_type_Lua_LLThread_check(L,1);
  bool res_idx1 = 0;
  const char * err_msg_idx2 = NULL;
	Lua_LLThread_child *child;
	char buf[ERROR_LEN];
	int top;
	int rc;

	if((this_idx1->state & TSTATE_STARTED) == 0) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Can't join a thread that hasn't be started.");
		return 2;
	}
	if((this_idx1->state & TSTATE_DETACHED) == TSTATE_DETACHED) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Can't join a thread that has been detached.");
		return 2;
	}
	if((this_idx1->state & TSTATE_JOINED) == TSTATE_JOINED) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Can't join a thread that has already been joined.");
		return 2;
	}
	/* join the thread. */
	rc = llthread_join(this_idx1);
	child = this_idx1->child;

	/* Push all results after the Lua code. */
	if(rc == 0 && child) {
		if(child->status != 0) {
			const char *err_msg = lua_tostring(child->L, -1);
			lua_pushboolean(L, 0);
			lua_pushfstring(L, "Error from child thread: %s", err_msg);
			return 2;
		} else {
			lua_pushboolean(L, 1);
		}
		top = lua_gettop(child->L);
		/* return results to parent thread. */
		llthread_push_results(L, child, 2, top);
		return top;
	} else {
		res_idx1 = false;
		err_msg_idx2 = buf;
		strerror_r(errno, buf, ERROR_LEN);
	}

  lua_pushboolean(L, res_idx1);
  lua_pushstring(L, err_msg_idx2);
  return 2;
}

/* method: new */
static int llthreads__new__func(lua_State *L) {
  size_t lua_code_len_idx1;
  const char * lua_code_idx1 = luaL_checklstring(L,1,&(lua_code_len_idx1));
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Lua_LLThread * this_idx1;
	this_idx1 = llthread_create(L, lua_code_idx1, lua_code_len_idx1);

  obj_type_Lua_LLThread_push(L, this_idx1, this_flags_idx1);
  return 1;
}



static const luaL_reg obj_Lua_LLThread_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_reg obj_Lua_LLThread_methods[] = {
  {"start", Lua_LLThread__start__meth},
  {"join", Lua_LLThread__join__meth},
  {NULL, NULL}
};

static const luaL_reg obj_Lua_LLThread_metas[] = {
  {"__gc", Lua_LLThread__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Lua_LLThread_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Lua_LLThread_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Lua_LLThread_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const luaL_reg llthreads_function[] = {
  {"new", llthreads__new__func},
  {NULL, NULL}
};

static const obj_const llthreads_constants[] = {
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_Lua_LLThread), 0, obj_Lua_LLThread_pub_funcs, obj_Lua_LLThread_methods, obj_Lua_LLThread_metas, obj_Lua_LLThread_bases, obj_Lua_LLThread_fields, obj_Lua_LLThread_constants},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL}
};







static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_llthreads(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

#if LUAJIT_FFI
	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
#endif

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
	luaL_register(L, "llthreads", llthreads_function);

	/* register module constants. */
	obj_type_register_constants(L, llthreads_constants, -1);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	nobj_try_loading_ffi(L, "llthreads", llthreads_ffi_lua_code,
		llthreads_ffi_export, priv_table);
#endif
	return 1;
}


