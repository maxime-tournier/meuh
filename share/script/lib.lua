local lib = {} 

function lib.load( name )
	 local loader =  package.loadlib( name, 'init' )
   if not loader then
      error( "coulnd't load library " .. name .. '!' )
      -- this should be fatal !
   else 
			loader()
      print('loaded ' .. name)
   end
end


return lib