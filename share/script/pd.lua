local pd = {}

function pd.update(what)

   if not what.set then 
      error("no set member lol") 
   end
   
   local error = what.desired - what.current

   local delta = 0;
   if(what.delta) then 
      delta = what.delta;
   end
   
   what.delta = what.p * error + what.d * delta;
   what.current = what.current + what.delta
   
   what.set(what.current)
end


return pd

