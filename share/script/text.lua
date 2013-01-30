require 'meuh'
require 'hook'

local viewer = require 'viewer'
local text = {}
local key = require 'key'

text.value = ''

-- displays a text in the viewer for @duration seconds(?)
function text.go(value, duration)
   duration = duration or 2
   
   text.value = tostring(value)

   -- end time
   text.last = viewer.time() + duration
   
end

function text.hook()
   if text.value and text.last and viewer.time() <= text.last then
      viewer.text( text.value )
   else 
      text.last = nil
   end
end

key['\b'] = function() text.go('') end

viewer.draw.text = text.hook 

return text
