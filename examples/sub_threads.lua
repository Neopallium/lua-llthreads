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

local llthreads = require"llthreads"
 
--- level 0 string literal enclosure [[  ]]
local thread_code = [[  
    print("CHILD: received params:", ...)    
    local llthreads = require"llthreads" -- need to re-declare under this scope
    local t = {} -- thread storage table
    -- create a new child thread - with level 1 literal string [=[ ]=] enclosures, level 2 would be [==[ ]==]
    local executed_child_code = [=[ 
        return 'Hello from child sub-thread, new input params:', ... 
    ]=]
    -- first child sub-thread
    local thread1 = llthreads.new(executed_child_code , "number:", 1001, "nil:", nil, "bool:", true) 
    assert(thread1:start()) -- start new child sub-thread
    table.insert(t, 1, thread1) -- insert thread into thread table
    -- second child sub-thread
    local thread2 = llthreads.new(executed_child_code , "number:", 1002, "nil:", nil, "bool:", true) 
    assert(thread2:start()) -- start new child sub-thread
    table.insert(t, 1, thread2) -- insert thread into thread table
    -- wait for all child sub-threads to complete before returning to root
    while true do        
        print("PARENT: child returned: ", t[1]:join()) 
        table.remove(t,1)
        if (#t <= 0) then break end
    end
    return ...  -- return the parents' input params back to the root
]]

-- create child thread.
local thread = llthreads.new(thread_code, "number:", 1234, "nil:", nil, "bool:", true)
-- start joinable child thread.
assert(thread:start())
print("Root: child returned: ", thread:join())
