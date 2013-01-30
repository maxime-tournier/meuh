

load['playback'] =   
   function()
   
      app.gui( false )
      app.speed:set(1)
      app.dt:set(0.01)
      
      app.control:set( false )
      app.contacts:set( false )
      app.limits:set( false )
      app.gravity:set( false )
      app.inertia:set( false )
     
      viewer().resize(512, 512)
      
      video.dt( app.dt:get() )
      video.video = 8
      
      viewer().animate( true )
      text.go( bvh.current.subject.. '/'..bvh.current.trial )
   end


animate['playback'] = 
   function()
      local t = viewer().time()
      app.playback( t )
      
   end


keys.n = function() bvh.load( bvh.next() ) end



function help()
   print('type animate(i), with i=0..14 to animate principal geodesic i')
end


