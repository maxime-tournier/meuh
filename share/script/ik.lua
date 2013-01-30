



custom = {}


custom ['82_13'] = 
   function()

      app.speed:set(0.1)
      app.length(34)
      
      app.track( app.dof('lfoot'), -1)
      app.track( app.dof('rfoot'), -1)
      app.track( -1, -1)

      app.set_geo(12)
   end


load['ik'] = 
   function()
      app.control:set( false )
      app.gravity:set( false )
      app.inertia:set( true )
      app.momentum:set( false )
      app.contacts:set( false )
      app.damp_model:set( 1 )

      app.limits:set( true )

      app.stiffness():set( 100 )
      app.length(10)

      customize(custom)
      viewer().animate( true )
   end



restart['ik'] = 
   function()
      app.cam(1)
   end


