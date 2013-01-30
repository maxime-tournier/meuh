local complete = {}


function complete.global( name ) 

   local res = {}
   
   for i,j in pairs(_G) do
      
      if type(i) == 'string' then
	 
   	 if string.find(i, '^'..name) then 
   	    table.insert( res, i )
   	 end

      end
   end
   
   return res
end



return complete