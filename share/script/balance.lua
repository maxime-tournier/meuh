
viewer = require 'viewer'

function on_load()
   print('balance load')
   
   app.walk(); update()
   
   app.control:set( true )
   app.friction:set( false )
   
   -- unactuated coordinates dimension (leading ?)
   app.unactuated:set(0)

   -- modal damping/stiffness
   app.damping():set( 400 )
   app.stiffness():set( 10 )

   app.push_stiff:set( 2000 )

   app.laziness:set(0)
   app.reference:set(0.001)
   app.am:set(1.5)
   
   -- angular limits
   app.limits:set( true )
   
   -- time step
   app.dt:set(0.01)

   -- modal space dimension
   app.set_geo( 14 )

   -- modal actuation bounds
   app.strength:set(3000)

   -- target viewer fps
   viewer.fps(60)

   -- reinit the simulation
   app.restart()

   -- app.ccsk()
   
end

-- loading callback
load['balance'] = on_load


move = require 'move'

-- animation callback ('flight' is a callback handle)
animate['flight'] = 
   function()
      if not app.has_contacts() and cpp.app().land then
	 cpp.app().land()
      end
   end

-- adds an animate callback, with handle 'toto', that prints toto
animate['toto'] = function() print('toto') end


app.debug( true )


