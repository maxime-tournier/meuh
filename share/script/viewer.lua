
require 'hook'

local api = require 'api'
local viewer = api.load('viewer')
local key = require 'key'

viewer.draw = {}
viewer.init = {}

key.doc.u = 'straighten view'
key.u = viewer.straighten

key.doc['*'] = 'restore viewer state from xml file'
key['*'] = viewer.restore


-- wraps @f so that it gets called after next draw
function viewer.wrap( f ) 
   
   return function( ... )
	     local args = { ... }
	     viewer.draw[f] = function()
				 f( unpack(args) )
				 viewer.draw[f] = nil
			      end
	  end
   
end


return viewer
