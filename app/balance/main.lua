-- modules includes
viewer = require 'viewer'
sim = require 'simulator'

gl = require 'gl'


-- submodules
app.char = {}
app.light = {}
app.params = {}

local nan = 0 / 0



local function with_text( name )
   local f = api.hide(app, name)
   
   return function(...)
	     f(...)
	     text.go(name .. ': '.. tostring(...) )
	  end
end

local function vec3( f )

   return function(c)
	     return  f(c[1], c[2], c[3])
	  end 
   
end


local exclude = api.hide(app, 'exclude')

function app.exclude( names )
   exclude('')
   
   for i,v in ipairs(names) do
      exclude(v)
   end
   
end

-- character
app.char.color = vec3( api.hide(app, 'char_color') )
app.char.pos = vec3( api.hide(app, 'pos') )



-- light
app.light.pos = vec3( api.hide(app, 'light_pos'))
app.light.lookat = vec3( api.hide(app, 'light_lookat'))
app.light.draw = api.hide(app, 'light_draw')

-- update light lookat after each frame
function app.light.target() 
   return {0, 0, 0} 
end

sim.post_animate.light = function() 
			    app.light.lookat( app.light.target() )
			 end





-- parameters
local function config( p ) 
   
   for k,v in pairs(p) do
      app[k](v)
   end
   
end

-- applies params
function app.update()
   config( app.params )
end

-- displays param values
function app.show()
   for k,v in pairs(app.params) do
      print(k, v)
   end
end


-- update params after each frame
sim.post_animate.update = app.update



function app.bvh(subject, trial, modes)

   local strial = ''
   
   if trial < 10 then 
      strial = '0'
   end
   
   strial = strial..trial
   
   app.load('bvh/'..subject..'/'..subject..'_'..strial..'.bvh',
	    modes)

end


app.bretelles = with_text('bretelles')
app.gravity = with_text('gravity')

