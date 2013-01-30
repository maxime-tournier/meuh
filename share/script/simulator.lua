local api = require 'api'

local sim = api.load('simulator')

require 'meuh'
require 'hook'

local text = require 'text'
local key = require 'key'
local viewer = require 'viewer'

require 'toboolean'

function sim.correct(val)
   sim.correction:set( toboolean(val) ) 
   text.go("full proj: "..tostring(val))
end

key.r = function() sim.reset() end
key.c = key.toggle( sim.correct, false )


function sim.timestep( x )
   sim.dt:set( x )
   text.go("dt: "..sim.dt:get() ) 
end

key["+"] = function() 
	      sim.timestep( 2*sim.dt:get() ) 
	    end

key["-"] = function() 
	      sim.timestep( sim.dt:get() / 2 )
	   end

local function toggle_time( val ) 
   if val then 
      sim.post_animate.show_time = function() text.go( string.format("%.2f", sim.t:get() ), 
						       sim.dt:get() ) 
				   end
   else 
      sim.post_animate.show_time = nilx
   end
end

key["t"] = key.toggle( toggle_time, true )

for i = 1,9 do
   local k = ''..i;
   key[k] = function() 
	       text.go("step "..i)
	       sim.step(i) 
	    end
end

sim.post_animate = {}
sim.post_reset = {}

-- makes the simulator yield the coroutine @co while in post_animation
-- every @duration seconds (simulation time), until the coroutine is
-- dead.
function sim.every(duration, co) 
   local bound = sim.t:get()

   sim.post_animate[co] = function ()
			     if sim.t:get() >= bound then
				local ok, ret = coroutine.resume(co)
				if not ok then error( ret ) end
				
				bound = sim.t:get() + duration
				
				if coroutine.status(co) == 'dead' then
				   sim.post_animate[co] = nil
				end
				
			     end
			  end
   
end


-- convenience function to do some stuff only once
function sim.after(duration, fun)
   
   local co = coroutine.create( function() 
				   coroutine.yield()
				   fun();
				end )
   
   sim.every(duration, co )
end


function sim.abort( co )
   if co then sim.post_animate[co] = nil end
end


function sim.start() viewer.animate( true ) end
function sim.stop() viewer.animate( false ) end

function sim.safe( action ) 
   stop()
   action()
   start()
end


-- given a coroutine, simulate for @duration and place log.data into
-- result, indexed by coroutine return
function sim.benchmark(duration, what, after) 
   assert( duration, what )

   local res = { dt = sim.dt:get(), data = {} }
   
   local co = coroutine.create( function() 

				   -- start
				   local ok, ret = coroutine.resume( what )
				   
				   while ret do
				      if not ok then error( ret ) end
				      
				      -- simulate for @duration
				      coroutine.yield()
				      
				      res.data[ret] = log.data
				      
				      -- next iteration
				      ok, ret = coroutine.resume(what)
				   end

				   if after then after(res) end
				end )

   sim.every(duration, co)

   return res
end



local post_steps = 2;

-- TODO this should go to simulator.lua
key.s = function()
	   post_steps = post_steps - 1
	   if( post_steps < 0 ) then post_steps = 0 end
	   sim.post_steps( post_steps )
	   text.go("post-steps: "..post_steps)
	end

key.S = function()
	   post_steps = post_steps + 1
	   sim.post_steps( post_steps )
	   text.go("post-steps: "..post_steps)
	end

old = {}
old.background = sim.background

-- performs animation in a background thread (less efficient :-/)
function sim.background(b)
   old.background(b)
   text.go("background: "..tostring(b) )
end

key.doc.b = 'performs the animation loop in a background thread (less efficient :-/)'
key.b = key.toggle(sim.background, true)

return sim

