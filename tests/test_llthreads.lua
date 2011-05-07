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

local function detached_thread(...)
	local thread = llthreads.new([[ print("print_detached_thread:", ...) ]], ...)
	-- start detached thread
	assert(thread:start(true))
	return thread
end

local function print_thread(...)
	local thread = llthreads.new([[ print("print_thread:", ...); ]], ...)
	-- start joinable thread
	assert(thread:start())
	return thread
end

local function pass_through_thread(...)
	local thread = llthreads.new([[ return "pass_thread:", ... ]], ...)
	-- start joinable thread
	assert(thread:start())
	return thread
end

local thread1 = detached_thread("number:", 1234, "nil:", nil, "bool:", true)

sleep(1)

local thread2 = print_thread("number:", 1234, "nil:", nil, "bool:", true)
print("thread2:join: results # = ", select('#', thread2:join()))

sleep(1)

local thread3 = pass_through_thread("number:", 1234, "nil:", nil, "bool:", true)
print("thread3:join:", thread3:join())

sleep(1)

