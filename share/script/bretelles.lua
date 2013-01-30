move = require 'move'

function on_load()

   app.set_geo( 14 )
   app.restart()
   -- app.ccsk()

   app.walk(); 
   update()

   app.control:set( true )
   app.bretelles:set( true )
   
   app.damping():set( 400 )
   app.stiffness():set( 500 )

   app.push_stiff:set( 2000 )

   app.reference:set(0.01)
   app.limits:set( true )

   viewer().fps(60)
   
   
end


load['bretelles'] = on_load

app.debug( true )


animate['flight'] = 
   function()
      if not app.has_contacts() then
	 cpp.app().land()
      end
   end







-- app.debug( true )