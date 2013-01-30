
local tree = {}



-- non-number key,value iterator
local function spairs(t) 
   
   local next, state, current = pairs(t)

   
   local function new_next(t, state) 
      
      local k, v = next(t, state)

      while tonumber(k) do
	 k, v = next(t, k)
      end

      return k, v
   end
   
   return new_next, state, current
   
end

-- node attributes (key, value)
function tree.attr(t) 
   return spairs(t)
end

-- children iterator (index, child)
function tree.sub(t) 
   return ipairs(t)
end



function tree.prefix( t, fun ) 

   fun( t )
   
   for i,c in ipairs(t) do
      tree.prefix(c, fun)
   end
   
end


function tree.postfix( t, fun ) 
   
   for i,c in tree.sub(t) do
      tree.prefix(c, fun)
   end
   
   fun( t )
   
end

function tree.push(t, c) 
   table.insert(t,  c)
end

function tree.print(t, indent) 
   
   indent = indent or 0;
   
   for k,v in tree.attr(t) do
      
      for i = 1, indent do
	 io.write(' ')
      end
      
      print(k, v )
      
   end


   for i,c in tree.sub(t) do
      tree.print(c, indent + 1)
   end
   
end


local function test() 

   t = { sponge = 'bob' }

   tree.push( t, { michelle = 'ma belle' } )
   tree.push( t, { derp = 'herp' } )

   tree.print( t )
end

return tree

