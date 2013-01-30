local viewer = require 'viewer'

local log = { }

log.data = {}

function log.now() 
   local t = viewer.time()

   if not log.data[ t ] then
      log.data[ t ] = {} 
   end
   
   return log.data[ t ]
end


function log.clear()
   log.data = {}
end

return log

