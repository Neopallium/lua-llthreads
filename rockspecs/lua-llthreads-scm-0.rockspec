package = "lua-llthreads"
version = "scm-0"
source = {
	url = "git://github.com/Neopallium/lua-llthreads.git",
}
description = {
	summary = "Low-Level threads for Lua",
	homepage = "http://github.com/Neopallium/lua-llthreads",
	license = "MIT/X11",
}
dependencies = {
	"lua >= 5.1, < 5.3",
}
build = {
	type = "builtin",
	platforms = {
		unix = {
			modules = {
				llthreads = {
					libraries = {"pthread"},
				}
			}
		}
	},
	modules = {
		llthreads = {
			sources = {"src/pre_generated-llthreads.nobj.c"},
		}
	}
}
