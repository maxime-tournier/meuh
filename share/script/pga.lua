require 'meuh'
require 'keys'

video = require 'video'
text = require 'text'

function animate(i)
   return function()
	     local scale = 1.5
	     for j = 1, 3 do
		local value = 0
		if j == i then
		   value = scale * math.sin( app().time:get() )
		end
		app().coord(j - 1):set( value * app().eigen(j - 1):get() / app().eigen(0):get()  )
	     end
	     
	  end 
end



function record( val )
   app().record(val)

   if val then 
      draw_mode = nil
      text.go("Recording...", 10) 
   else 
      text.go("done.", 10) 
   end

end


function play()
   text.go("Input data", 10)
   app().play()
end


keys.r = toggle(record , true )
keys.p = play

for i = 1,3 do
   keys[''..i] = function() 
		    text.go("Mode "..i, 10)
		    anim = animate(i)
		    draw_mode = function() app().mode( i - 1) end
		 end
end




function post_animation()
   hook( anim )
   hook( video.hook )
end

function post_draw()
   hook( draw_mode )
   hook( text.hook )
end


app().dt:set(0.01)


where = '/home/max/tmp/pga'
