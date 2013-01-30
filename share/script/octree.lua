local api = require 'api'

local octree = api.load('octree')
local key = require 'key'
local text = require 'text'
local sim = require 'simulator'

local old = {}

old.add = octree.add

function octree.add()
   text.go( tostring(old.add()) )
end

-- sim time between ball creations
octree.speed = 0.05

local rain_co = nil

function octree.rain(count)
   if rain_co then sim.abort( rain_co ) end
   
   local co = coroutine.create( function () 
				   
				   for i=1,count do
				      octree.add()
				      coroutine.yield()
				   end
				   rain_co = nil
				end)
   rain_co = co
   sim.every( octree.speed , co)
end

old.solver = octree.solver

local names = { "MINRES", "PGS" }

function octree.solver(t) 
   text.go( tostring(names[t]) )
   if t > 2 then t = 2 end
   old.solver(t)
end


key.z = function() 
	   octree.add() 

	   -- i *love* lua
	   
	   local old = key.z
	   local co = coroutine.create( function() 
					   key.z = nil
					   coroutine.yield()
					   key.z = old
					end)
	   sim.every(octree.speed, co)

	end

key.x = function() octree.rain(150) end

key.s = key.cycle(octree.solver, 1, 2, 2)

key[' '] = key.toggle(octree.walls)

old.warm = octree.warm

function octree.warm(b)
   old.warm(b)
   text.go( 'warm: '..tostring(b))
end

key.doc.w = 'warm start on/off'
key.w = key.toggle(octree.warm, true)

function octree.post_init()
   
   sim.post_reset.abort_rain = function() 
				  sim.abort( rain_co )
			       end
   old.solver( 1 )
   old.warm( false )
   old.correct( false )

   octree.iter( 10 )
   octree.eps( 1e-3 )
   
   video = require 'video'
end

old.correct = octree.correct

function octree.correct(b)
   old.correct(b)
   text.go("correct: "..tostring(b))
end

key.c = key.toggle(octree.correct, true)

return octree


