About
=====

A simple low-level Lua wrapper for pthreads.

Example usage
=============

	local llthreads = require"llthreads"

	local thread_code = [[
		-- print thread's parameter.
		print("CHILD: received params:", ...)
		-- return all thread's parameters back to the parent thread.
		return ...
	]]
	
	-- create child thread.
	local thread = llthreads.new(thread_code, "number:", 1234, "nil:", nil, "bool:", true)
	-- start joinable child thread.
	assert(thread:start())
	print("PARENT: child returned: ", thread:join())


