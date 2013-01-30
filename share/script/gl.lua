
local api = require 'api'
local gl = api.load 'gl'

local viewer = require 'viewer'


-- schedules a functions on next draw
local scheduled = viewer.wrap

-- hides the original 3-args c++ and provides a table version instead
local function vec3( f )
   
   return function(c)
	     f(c[1], c[2], c[3])
	  end 
   
end


local function sched3( name ) 
   return scheduled( vec3( api.hide(gl, name ) ) )
end

local function sched( name )
   return scheduled( api.hide(gl, name ) )
end


gl.clear = {}
gl.clear.color = sched3('clear_color')


gl.fog = {}
gl.fog.color = sched3('fog_color')
gl.fog.density = sched('fog_density')

gl.ambient = sched3('ambient')



function gl.html( c ) 

   local byte = 256

   local b = c % byte
   c = c / byte
   
   local g = c % byte
   c = c / byte
   
   local r = c % byte

   return {r / 256, g / 256 , b / 256}
end


return gl

