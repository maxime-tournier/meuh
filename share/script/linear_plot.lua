

linear = require 'linear'
pretty = require 'pretty'

local cg  = {}

require 'serialize'

local cg = {}

function cg.iter ( x ) 
   
   return x and x.cg.iter, "CG Iterations";
end

local function time ( x ) 
   return x and x.time, "Solve Time";
end



function plot( file, what )
   what = what or cg.iter

   local res = io.load(file) or error("fuuu")
   
   
   linear.plot(res.data, what)

end



plot( '/home/max/Documents/results/2012-01-17-11:00:04', cg.iter )
