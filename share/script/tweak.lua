
local api = require 'api'
local tweak = api.load 'tweak'
local viewer = require 'viewer'



tweak.real = viewer.wrap( api.hide(tweak, 'real') )


return tweak