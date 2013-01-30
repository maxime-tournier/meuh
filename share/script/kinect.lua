local api = require 'api'

local kinect = api.load('kinect')

local key = require 'key'


key.r = function() 
	   kinect.rgb()
	end


key.i = function() 
	   kinect.ir()
	end

key.s = function() 
	   kinect.save()
	end

