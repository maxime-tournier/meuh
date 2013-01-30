
function set_mode(i, value)
   for j =0,app.get_geo() - 1 do
      if( i == j ) then
	 app.coord(j):set( value )
      else
	 app.coord(j):set( 0 )
      end
   end
end

function animate_mode(i)
   
   animate['modes'] = 
      function() 
	 local scale = 1.2
	 set_mode(i, scale * math.sin( cpp.app().time:get()) )

      end
   
   text.go("Mode  #" .. (i+1), 20)
   
end


function on_load() 
    
   app.set_geo( 15 )
   app.restart();
   
   -- vars
   app.control:set( false )
   app.contacts:set( false )
   app.unactuated:set( 0 )
   app.gravity:set( false )
   app.dt:set(0.01)
   app.fixed:set( true )
   app.inertia:set( false )
   
   -- func
   app.gui( false )
   viewer().resize( 512, 512 )
   app.length( 10000  )

   app.debug( true )
   app.euler:set(false);

   -- go 
   animate_mode(0)
   
   keys.e = toggle( function( value ) 
		       app.euler:set(value); 
		       if(value) then text.go('Euler', 10) else text.go('PGA', 10) end; 
		    end, false);

end


load['modes'] = on_load


function help()
   print('type animate(i), with i=0..14 to animate principal geodesic i')
end


for i = 0,9 do
   local key = ""..i
   keys[ key ] = function() animate_mode(i) end
end

function table.pop_front(t)
   local i = next(t)
   local res = t[ i ]
   t[ next(t) ] = nil
   return res
end


function go() 
   modes = {0, 1, 2, 3, 4}
   animate_mode( table.pop_front(modes) )
   app.restart()
   -- viewer().animate( true )

   animate['go'] = function()
		      if viewer().time() > 12.5 then 
			 if #modes > 0 then 
			    animate_mode( table.pop_front(modes) )
			    app.restart()
			 else
			    viewer().animate(false)
			 end
		      end
		   end

end

app.euler:set( false )
keys.x = go
