
-- simple dkjson wrapper for serialization

local rocks = require 'rocks'

local lpeg = rocks.require('lpeg')
local json = rocks.require('dkjson').use_lpeg()


local ascii = require 'ascii'

local res = {}


function res.load( filename ) 
   local str = ascii.load( filename )

   local obj, pos, err = json.decode (str, 1, nil)

   if err then
      error ("json error:", err)
   else
      return obj
   end
end

function res.save( filename, obj )
   local str = json.encode (obj, { indent = true })
   
   ascii.save(filename, str)
end


return res
