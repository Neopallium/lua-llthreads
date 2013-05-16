local llthreads = require "llthreads"

local CODE=[[
local function gc(fn)
  local p = assert(newproxy())
  assert(debug.setmetatable(p, { __gc = fn }))
  return p
end 

p = gc(function() print("hello from gc") end)
]]

local thread = llthreads.new(CODE)
thread:start()
thread:join()

