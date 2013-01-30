
-- -- drop character
-- drop_time = 20

-- function drop() 
--    app[ app.selected() .. '_soft' ]:set( app.time:get() > drop_time )
-- end

require 'meuh'
require 'hook'


text = require 'text'
video = require 'video'
keys = require 'key'
bvh = require 'bvh'

cpp = {app = control}

-- updates c++ api on the lua side
function update()
   app = cpp.app()
end

-- displays current modal coordinate values
function log_coords() 
   local out = "coordinates:";
   for i=0,app.get_geo()-1 do
      out = out .. " "..app.coord(i):get();
   end
   app.log( out )
end

-- ha !
function help () 
   print("no help defined for this script")
end

-- greeter
function welcome()
   print("type 'help()' to get help, or 'quit()' to quit.")
end

-- 
function quit() 
   app.quit()
end


-- applies customization for current filename
function customize( pack ) 
   -- gets current filename
   local name = app.filename()
   
   -- if the key pattern is matched, applies corresponding fun
   for pattern, fun in pairs(pack) do
      if string.find(name, pattern) then fun(); return true; end 
   end
   
   return nil;
end

-- set mean pose (modal coordinates => 0)
function mean_pose()
   local n = app.get_geo()	-- geodesic count
   for i = 1,n do
      app.coord(i-1):set(0)
   end				-- sets coordinates to 0
end

-- hooks
animate = {}
draw = {}
load = {} 
restart = {} 

-- default hooks
animate['video'] = video.hook	
draw['text'] = text.hook 

-- bvh database initialization
bvh.init()

-- refresh api handle
update()

-- app.debug( false )


-- key bindings customization

-- toggles gui on/off
keys.m = toggle( app.gui, false )

-- displays help on the console
keys["?"] = help

-- saves current pose as starting pose
keys.i = function() app.save(); text.go("Saved") end

-- toggles camera traveling on/off
keys.t = toggle( function(value) app.travelling:set(value) end, true)

-- toggles angular limits on/off
keys.l = toggle( function(val) app.limits:set( val ) end, true )

-- activates first camera on restart (defined using F1 / ctrl + F1 ?)
--  see QGLViewer
restart['cam'] =
   function()
      app.cam(1)
   end

-- available scripts:
-- balance: full control
-- bretelles: fake control
-- fall: inclined plane
-- ik ? maybe broken
-- modes: animates modes
-- tracking ?

-- launch meuh/bin/control -e foo to start with script foo
if not script then 
   script = 'balance' 
end

-- loads script
require(script)

-- hello
welcome()

