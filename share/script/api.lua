local api = {}

-- api.quiet = true

function api.add( name, fun )
   if not api.quiet then 
      print("adding api "..name)
   end
   
   api[name] = fun 
end

function api.load( name )
   if not api[name] then 
      print('warning: api', name, 'not loaded!')
      api[name] = {}
   end
   return api[name]()
end


api.hidden = {}

function api.hide(mod, fun )
   local res = mod[fun]
   mod[fun] = nil
   api.hidden[fun] = res
   return res
end


return api

