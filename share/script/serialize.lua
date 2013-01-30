


local function serialize (o)
   if type(o) == "number" then
      io.write(o)
   elseif type(o) == "string" then
      io.write(string.format("%q", o))
   elseif type(o) == "table" then
      -- io.write("(function() local res = {}; ");
      -- for k,v in pairs(o) do
      -- 	 io.write("res[");
      -- 	 serialize(k)
      -- 	 io.write("] = ")
      -- 	 serialize(v)
      -- 	 io.write(';\n')
      -- end
      -- io.write("return res; end)()")
      
      io.write("{\n")
      for k,v in pairs(o) do
      	 io.write("  [")
      	 serialize(k)
      	 io.write("] = ")
      	 serialize(v)
      	 io.write(",\n")
      end
      io.write("}\n")
   else
      error("cannot serialize a " .. type(o))
   end
end


function io.load(file) 
   local f = io.open( file, 'r' )
   if not f then return nil end
   
   -- local f = assert( loadfile(file) )
   -- return f()
   
   local cmd = "return "..f:read("*all")
   local fun = assert( loadstring(cmd) )
   f:close()
   
   return fun()
end



function io.save(file,  what)

   io.output( file )
   serialize(what)
   io.close()

end

