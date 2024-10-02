package = "lua-llthreads"
version = "1.3-1"
source = {
	url = "git://github.com/Neopallium/lua-llthreads.git",
	branch = "v1.3",
}
description = {
	summary = "Low-Level threads for Lua",
	homepage = "http://github.com/Neopallium/lua-llthreads",
	license = "MIT/X11",
}
dependencies = {
	"lua >= 5.1, < 5.5",
}
build = {
	type = "builtin",
	modules = {
		llthreads = {
			sources = {"src/pre_generated-llthreads.nobj.c"},
			libraries = {"pthread"},
		}
	}
}
