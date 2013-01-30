viewer = require 'viewer'

function on_load() 
   print( 'load')
   app.set_geo( 12 )
  
 
   app.restart();

    -- vars
   app.control:set( false )
   app.unactuated:set( 0 )

   app.gravity:set( true )
   app.travelling:set( true )

   app.correct:set( false )

   app.show_momenta:set( false )
   app.nappe:set( false )
   app.trace:set( false )

   -- func
   app.gui( false )
   
   app.incline(0.8)
   app.mu:set(1)
   app.contacts:set( true )
   app.stiffness():set(200.0)

   app.dt:set(0.01)
   app.damping():set( 100 )
   app.damp_model:set(1)

   -- app.show_limit( app.dof('lclavicle'))
end



function approx_name() 
   local i = app.approx:get()
   if i == 0 then return "normal" end
   if i == 1 then return "small" end
   if i == 2 then return "constant" end
   return "error"
end


function on_restart() 
   print( 'restart')
   app.push_stiff:set( 100000.0 )
   viewer.resize( 512, 512 )
   
   video.record( record )
   
   -- local head = app.dof('head');
   -- app.push(head, false)
   
   text.go(approx_name(), 40)
   app.length( length )
   
   app.limits:set( true )
   
   app.dt:set( dt )
   video.dt( dt )
   
   -- auto-refresh
   -- refresh('hanging')
end


function help()
   print('type approx(i), with i=0,1,2 to compare integrator approximations')
end


function approx(i)
   
   app.approx:set(i - 1)
   app.restart()
end

for i = 1,3 do
   local key = ""..i
   keys[ key ] = function() approx(i) end
end


function finish()
   app.animate( false )
   if( record ) then
      video.make( record )
      video.clean( record )
      video.show( record )
      next()
      
   end
  
end


function post_animation()
   if( app.time:get() >= length ) then 
      finish()
   end

end

animate['hanging'] = post_animation
load['hanging'] = on_load
restart['hanging'] = on_restart

app.debug( false )


basedir = '/home/max/tmp'

normal = basedir..'/hanging-normal'
small = basedir..'/hanging-small'
constant = basedir..'/hanging-constant'

record = nil
to_record = {}

keys.p = toggle( function(val) app.show_momenta:set(val) end, false )
keys.t = toggle( function(val) app.travelling:set(val) end, false )


length = 500.0
dt = 0.01

function go()
   to_record = {normal, small, constant} 
   next() 
end

function next()
   if #to_record > 0 then
      record = to_record[#to_record]
      approx(#to_record)
      table.remove(to_record)
      app.restart()
      app.animate( true )
   else 
      record = nil
   end
end



restart['campush'] =
   function()
      app.cam(1)
      app.push( app.dof("head"), false)
   end
