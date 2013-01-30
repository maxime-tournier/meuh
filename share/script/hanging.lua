

function on_load() 
   print( 'load')
   app.set_geo( 12 )
  
 
   app.restart();

    -- vars
   app.control:set( false )
   app.unactuated:set( 0 )

   app.gravity:set( true )
   app.travelling:set( false )

   app.show_momenta:set( false )
   app.correct:set( true  )

   app.nappe:set( false )
   app.trace:set( true )

   -- func
   app.gui( false )
   
   app.travelling:set(false)
   app.mu:set(1)
   app.contacts:set( true )
   app.stiffness():set(200.0)
   app.damping():set( 0.1 )
   app.damp_model:set(1)

   app.limits:set( true )
   
end



function on_restart() 
   print( 'restart')

   viewer().resize( 512, 512 )
   
   -- text.go(approx_name(), 40)
   app.length( length )

   
   app.dt:set( dt )
   video.dt( dt )
   
   -- auto-refresh
   -- refresh('hanging')
end


function help()
   print('type approx(i), with i=0,1,2 to compare integrator approximations')
end


function finish()
   print( "i are finished lol" )
   app.animate( false )

   video.record(nil)
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

record = nil
to_record = {}


function pretty(val)
   if val then return "enabled" 
   else return "disabled" end
end


keys.p = toggle( function(val) app.show_momenta:set(val) end, false )
keys.t = toggle( function(val) app.travelling:set(val) end, false )
keys.l = toggle( function(val) text.go("Limits "..pretty(val)); app.limits:set(val) end, false)

length = 500.0
dt = 0.01


restart['cam'] =
   function()
      app.cam(1)
   end
