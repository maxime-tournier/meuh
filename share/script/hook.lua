
function hook( x ) 
   for key, fun in pairs(x) do
      fun()
   end
end


