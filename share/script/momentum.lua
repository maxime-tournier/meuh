
function on_load() 
   app.set_geo( 16 )
   app.damping():set( 2 )
 
   app.restart();

   -- vars
   app.control:set( false )
   app.contacts:set( false )
   app.unactuated:set( 0 )
   app.gravity:set( false )
   app.travelling:set( true )
   app.momenta:set( true )
   
   app.nappe:set( true )
   app.trace:set( true )

   -- func
   app.gui( false )


end


function approx_name() 
   local i = app.approx:get()
   if i == 0 then return "naive" end
   if i == 1 then return "small" end
   if i == 2 then return "constant" end
   return "error"
end


record = nil

function on_restart() 
     
   app.push_stiff:set( 1800 )
   
   viewer().resize( 1024, 600 )
   
   if( record ) then 
      video.record( record ) 
   else 
      video.record( nil )
   end
   
   local head = app.dof('head');
   app.push(head, false)
   text.go(approx_name(), 40)
   app.length( length )

end


function help()
   print('type approx(i), with i=0,1,2 to compare integrator approximations')
end


function approx(i)
   
   app.approx:set(i)
   app.restart()
end

for i = 1,3 do
   local key = ""..i
   keys[ key ] = function() approx(i - 1) end
end


function finish()
   app.animate( false )
   if( record ) then
      video.make( record )
      -- video.clean( record )
      video.show( record )
      record = nil
   end
   app.restart()
end


function post_animation()
   if( app.time:get() >= length ) then 
      finish()
   end
end


animate['momentum'] = post_animation
restart['momentum'] = on_restart
load['momentum'] = on_load

app.debug( false )


length = 200


