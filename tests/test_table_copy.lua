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

local llthreads = require"llthreads"

local sleep
local status, socket = pcall(require,"socket")
if status then
	sleep = function(secs)
		return socket.sleep(secs)
	end
else
	sleep = function(secs)
		os.execute("sleep " .. tonumber(secs))
	end
end

local dump_code = [==[
local function dump_recur(seen, obj, depth)
	local out
	local t = type(obj)
	-- if not a table just convert to string.
	if t ~= "table" then
		if t == "string" then
			return '"' .. obj .. '"'
		end
		return tostring(obj)
	end
	-- check if this table has been seen already.
	if seen[obj] then 
		return "Already dumped " .. tostring(obj)
	end
	seen[obj] = true
	-- restrict max depth.
	if depth >= 10 then
		return "{... max depth reached ...}"
	end
	depth = depth + 1
	-- output table key/value pairs
	local tabs = string.rep("  ",depth)
	local out = "{\n"
	for k,v in pairs(obj) do
		if type(k) ~= "number" then
			out = out .. tabs .. '[' .. dump_recur(seen, k, depth) .. '] = ' ..
				dump_recur(seen, v, depth) .. ',\n'
		else
			out = out .. tabs .. '[' .. k .. '] = ' .. dump_recur(seen, v, depth) .. ',\n'
		end
	end
	return out .. tabs:sub(1,-3) .. "}"
end

local obj = ...
local seen = {}
return dump_recur(seen, obj, 0)
]==]

local dump = loadstring(dump_code)

local child_code = [==[
local dump = loadstring[[
]==] .. dump_code .. [==[
]]
local args = ...

print("Child thread args:", dump(args))

-- return all values.
return ...
]==]

local function test_thread_value_copying(...)
	local args = {...}
	print("Main thread args:", dump(args))
	local thread = llthreads.new(child_code, args)
	-- start joinable thread
	assert(thread:start())

	local status, results = thread:join()
	print("Main thread results:", dump(results))
end

-- create some tables.
local a1 = { "a1" }
local a2 = { "a2" }
local a3 = { "a3" }
local a4 = { "a4" }
local b1 = { a1, a2, a3, a4 }
local b2 = { a1=a1, a2=a2, a3=a3, a4=a4 }

--
-- no loops
--
test_thread_value_copying(b1, b2)

local top = {}
-- self reference.
top.top = top
top[top] = top
-- nested reference.
top.sub1 = { sub2 = { sub3 = { top } } }

--
-- loops
--
test_thread_value_copying(top)

--
-- Test max depth
--
local outer = {}
for n=1,100 do
	outer = {outer}
end
local status, err = pcall(test_thread_value_copying,outer)
assert(not status, "Assertion failed: max depth test failed.")

