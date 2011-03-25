-- Copyright (c) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

local Lua_LLThread_type = [[
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
	pthread_t  thread;
	Lua_TState state;
} Lua_LLThread;

]]
object "Lua_LLThread" {
	sys_include "pthread.h",
	c_source(Lua_LLThread_type),
	c_source[[

#include <stdio.h>

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

static void *run_child_thread(void *arg) {
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

	/* joinable thread, do not destroy the child state, return it back to parent. */
	if(this->is_detached == 0) {
		return this;
	}
	/* thread is detached, so it must clean-up the child state. */
	llthread_child_destroy(this);
	return NULL;
}

static int llthread_start(Lua_LLThread *this, int start_detached) {
	Lua_LLThread_child *child;
	int rc;

	child = this->child;
	child->is_detached = start_detached;
	rc = pthread_create(&(this->thread), NULL, run_child_thread, child);
	if(rc == 0) {
		this->state = TSTATE_STARTED;
		if(start_detached) {
			this->state |= TSTATE_DETACHED;
			rc = pthread_detach(this->thread);
		}
	}
	return rc;
}

static int llthread_join(Lua_LLThread *this) {
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
}

static int llthread_move_values(lua_State *from_L, lua_State *to_L, int idx, int top, int is_arg) {
	const char *str;
	size_t str_len;
	int nvalues = 0;
	int n;

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
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		default:
			if (is_arg) {
				return luaL_argerror(from_L, n, "table/function/userdata/thread types un-supported.");
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

]],
	destructor {
		c_source "pre" [[
	Lua_LLThread_child *child;
]],
		c_source[[
	/* if the thread has been started and has not been detached/joined. */
	if((${this}->state & TSTATE_STARTED) == TSTATE_STARTED &&
			(${this}->state & (TSTATE_DETACHED|TSTATE_JOINED)) == 0) {
		/* then join the thread. */
		llthread_join(${this});
		child = ${this}->child;
		if(child && child->status != 0) {
			const char *err_msg = lua_tostring(child->L, -1);
			fprintf(stderr, "Error from non-joined thread: %s\n", err_msg);
			fflush(stderr);
		}
	}
	llthread_destroy(${this});
]]
	},
	method "start" {
		var_in{ "bool", "start_detached", is_optional = true },
		var_out{ "bool", "res" },
		c_source "pre" [[
	int rc;
]],
		c_source[[
	if(${this}->state != TSTATE_NONE) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Thread already started.");
		return 2;
	}
	if((rc = llthread_start(${this}, ${start_detached})) != 0) {
		lua_pushboolean(L, 0); /* false */
		lua_pushstring(L, strerror(rc));
		return 2;
	}
	${res} = true;
]]
	},
	method "join" {
		var_out{ "bool", "res" },
		var_out{ "const char *", "err_msg" },
		c_source "pre" [[
	Lua_LLThread_child *child;
	int rc;
]],
		c_source[[
	if((${this}->state & TSTATE_STARTED) == 0) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Can't join a thread that hasn't be started.");
		return 2;
	}
	if((${this}->state & TSTATE_DETACHED) == TSTATE_DETACHED) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Can't join a thread that has been detached.");
		return 2;
	}
	if((${this}->state & TSTATE_JOINED) == TSTATE_JOINED) {
		lua_pushboolean(L, 0); /* false */
		lua_pushliteral(L, "Can't join a thread that has already been joined.");
		return 2;
	}
	/* join the thread. */
	rc = llthread_join(${this});
	child = ${this}->child;

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
		int top = lua_gettop(child->L);
		/* return results to parent thread. */
		llthread_push_results(L, child, 2, top);
		return top;
	} else {
		${res} = false;
		${err_msg} = strerror(rc);
	}
]]
	},
}

