local bindings = {}

function bindings.load( lib )
   if( package.loadlib( lib, 'init' )() ) then
      error( "coulnd't load c++ bindings from " .. lib .. '!' )
      -- this should be fatal !
   else 
      print('loaded bindings from '..lib)
   end

end


return bindings
