-- Copyright (c) 2011 by Ross Anderson <ross_j_anderson@yahoo.com>
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

-- Sub-thread processing example in Lua using llthreads - 1,000 quick sub-thread execution

-- luajit sub_threads.lua

local llthreads = require"llthreads"

local num_threads = tonumber(arg[1] or 1000)
 
-- level 0 string literal enclosure [[  ]] of child execution code
local thread_code = [[  
    local num_threads = ...
    print("CHILD: received from ROOT params:", ...)    
    local llthreads = require"llthreads" -- need to re-declare this under this scope
    local t = {} -- thread storage table

    -- create a new child sub-thread execution code - it requires level 1 literal string [=[ ]=] enclosures, level 2 would be [==[ ]==]
    local executed_child_code = [=[ 
        return "Hello from child sub-thread, new input params:", ... 
    ]=]

    -- create 1000 sub-threads - which creates an incremental 30% / 20% utilization spike on the two AMD cpu cores
    print("CHILD: Create sub threads:", num_threads)
    for i=1,num_threads do
        -- create child sub-thread with code to execute and the input parmeters
        local thread = llthreads.new(executed_child_code , "number:", 1000 + i, "nil:", nil, "bool:", true) 
        assert(thread:start()) -- start new child sub-thread
        table.insert(t, thread) -- append the thread at the end of the thread table
    end

    -- wait (block) for all child sub-threads to complete before returning to ROOT
    while true do
        -- always wait on the first element, since order is not important        
        print("CHILD: sub-thread returned: ", t[1]:join()) 
        table.remove(t,1) -- always remove the first element
        if (#t == 0) then break end
    end
    return ...  -- return the parents' input params back to the root
]]

-- create child thread.
local thread = llthreads.new(thread_code, num_threads, "number:", 1000, "nil:", nil, "bool:", true)
-- start joinable child thread.
assert(thread:start())
-- wait for all child and child sub-threads to finish
print("ROOT: child returned: ", thread:join())
