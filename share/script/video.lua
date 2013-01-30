

local viewer = require 'viewer'
local sim = require 'simulator'

local video = {}


video.rate = {input = 200, output = 60}
video.quality = 8
video.extension = '.mp4'


local function check_path( path )
   local res = path;
   if not res then res = video.path end
   if not res then error("i need a path lol") end
   
   return res
end

function video.dt( value )
   video.rate.input = 1 / value
end

-- TODO remove ?
local function basename( path ) 
   local s, e, ret =  string.find(path,'/?([^/]*)/?$')
   return ret
end

function video.filename( path ) 
   return path .. video.extension
end



function video.record( path ) 
   video.path = path
   
   if video.path then 
	       

      video.dt( sim.dt:get() )
      os.execute('mkdir -p '.. video.path)
      video.clean( video.path )
      
      viewer.snapshot(video.path..'/shot')

      -- install post animate hook
      sim.post_animate.video = video.hook
   else 
      -- uninstall it
      sim.post_animate.video = nil
   end
end


function video.hook()
   if video.path then 
      viewer.snapshot_next()
   end
end


function video.make( path ) 
   path = path or video.path
   assert( path )

   sim.stop()
   
   local file = video.filename( path )
   
   -- remove existing version since console fucks up ffmpeg input
   os.execute('rm '.. file )
   
   local cmd = 
      'ffmpeg -f image2 -r '.. video.rate.input
      ..' -i '..path..'/shot-%04d.jpg -r '.. video.rate.output 
      ..' -qmax '..video.quality
      ..' '.. file

   print( cmd )
   os.execute( cmd )
   return video
end

function video.show( path )
   path = check_path( path )
   os.execute('vlc '.. video.filename(path) .. '&')
   return video
end

function video.clean( path )
   os.execute('rm '..path..'/*.jpg')
end



function video.go(duration, filename)
   
   filename = filename or '/tmp/video'

   local stop = coroutine.create( function() 
				     video.record(filename)
				     
				     coroutine.yield()
				     
				     video.record()
				     
				     video.make( filename  )
				     video.show( filename  )
				     
				  end )
   
   sim.every( duration, stop )

   
end


return video

