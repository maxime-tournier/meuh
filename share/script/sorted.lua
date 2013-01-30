
-- taken from http://lua-users.org/wiki/SortedIteration

function sorted_index( t )
   local res = {}
   
   for key in pairs(t) do
      table.insert( res, key )
   end
   
   table.sort( res )
   return res
end


function sorted_next(t, state)
   -- Equivalent of the next function, but returns the keys in the alphabetic
   -- order. We use a temporary ordered key table that is stored in the
   -- table being iterated.
   local key
   
   --print("orderedNext: state = "..tostring(state) )
   if state == nil then
      -- the first time, generate the index
      t.__sorted = sorted_index( t )
      key = t.__sorted[1]
      return key, t[key]
   end
   -- fetch the next value
   key = nil
   for i = 1,table.getn(t.__sorted) do
      if t.__sorted[i] == state then
	 key = t.__sorted[i+1]
      end
   end
   
   if key then
      return key, t[key]
   end

   -- no more value to return, cleanup
   t.__sorted = nil
   return
end

function sorted_pairs(t)
   -- Equivalent of the pairs() function on tables. Allows to iterate
   -- in order
   return sorted_next, t, nil
end

