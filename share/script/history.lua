local api = require 'api'

local history = api.load('history')
local meuh = require 'meuh'

local old = {}

old.save = history.save
old.load = history.load

local default_filename = meuh.path()..'/.history'

function history.save(filename)
   filename = filename or default_filename
   old.save( filename );
end

function history.load(filename)
   filename = filename or default_filename
   old.load( filename );
end

return history