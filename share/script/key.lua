require 'meuh'

local key = {}

-- toggles a boolean function, with initial state
function key.toggle( fun, init ) 
   local state = init or false
   return function() 
	     fun( state )
	     state = not state
	  end
end

function key.press(k)
   if key[k] then key[k]() end
end


function key.cycle(fun, min, max, init) 
   local state

   if init then 
      state = init 
   else state = min 
   end
   
   return function() 
	     fun(state)
	     state = state + 1
	     if state > max then state = min end
	  end
end


-- fill this table with strings to document key shortcuts
key.doc = {}

function key.help() 
   
   for k,v in pairs(key.doc) do
      print(k, v)
   end

end


-- we need a global function to retrieve result from c++ (see
-- tool::viewer)
function key_press( k ) 

   local fun = key[k]

   if fun then fun(); return true
   else return false 
   end

end


return key



