
function refresh(what) 
   if( package.loaded[what] ) 
   then 
      package.loaded[what] = nil; 
      return require(what) 
   end
end