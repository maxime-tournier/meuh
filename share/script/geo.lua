require 'meuh'

key = require 'key'
video = require 'video'
text = require 'text'



app().dt:set(0.01)

function animate()
   hook( video.hook )
end

function draw()
   hook( text.hook )
end

key.r = app().reset

where = '/home/max/tmp/geo'
