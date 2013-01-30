

require 'keys'

for i = 1,9 do
   keys[""..i ] = function() 
		     app().level:set(i) 
		  end
end

