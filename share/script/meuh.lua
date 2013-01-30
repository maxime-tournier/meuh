local meuh = {}


function meuh.path()
   return os.getenv("MEUH_PATH")
end

-- display a bundle
function show( x ) 
   for name, value in pairs(x) do
      if( type(value) == "function" or value.__call ) then
	 print(name, 'function')
      else
	 print(name, value:get())
      end
   end
end


return meuh


