

function tree(level, arity) 
   arity = arity or 2
   
   local res = {}

   if level == 0 then return {lol = 'wat'} end
   
   for i = 1,arity do
      res['lol'..i] = tree( level - 1, arity) 
   end

   return res

end

pretty = require 'pretty'

require 'serialize'
json = require 'json'

function go( i, j )

   print('gen')
   huge = tree( i, j )

   print('new')
   json.save('/tmp/new', huge)
   new = json.load('/tmp/new')

   print('old')
   io.save('/tmp/old', huge)
   old = io.load('/tmp/old')

end

 