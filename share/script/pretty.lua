local pretty = {}


local function offset(start)
   
   for i = 1, start do
      io.write(' ')
   end

end

local function helper( what, start )
   
   if type(what) == "table" then
      for k,v in pairs(what) do
	 
	 helper(k, start)
	 helper(v, start + 3)
	 
      end
   else
      offset( start )
      print( what )
   end


end


function pretty.print( what ) 
   helper( what, 0 )
end


return pretty
