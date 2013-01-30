local api = require 'api'
local linear = api.load('linear')

local key = require 'key'
local plot = require 'plot'
local sim = require 'simulator'
local shell = require 'shell'

local text = require 'text'

-- require 'serialize'

local json = require 'json'

function linear.init() 
   linear.post( 2 )
   linear.iter( 30 )
   
   linear.setup(20, 10)
   linear.color( 0x334477 )
   
   sim.dt:set(8e-3)
end

key.doc.n = 'shows plots for last iteration'
key.n = function() 
	   
	   local last = 0
	   for t,v in sorted_pairs(test) do
	      last = t
	   end

	   plot.show( test[ last] ) 
	end

linear.init()

function linear.go(duration, name) 
   name = name or 'test'
   
   local save = function() 
		  json.save('/tmp/'..name, test)
	       end
   
   sim.reset()
   sim.start()
   sim.after( duration, save )
end

local function timestamp()
   return shell.exec("date +%F-%T")
end


function linear.bench(duration)

   local param = {}

   local big = 50
   local small = 10
   local mid = 0.5 * (big + small)

   param.horizontal = { w = big, h = small }
   param.square = {w = mid, h = mid }
   param.vertical = {w = small, h = big } 
   
   local res = {}
   
   local co = coroutine.create( 
      function()
	 for k, v in pairs(param) do
	    app.setup(v.w, v.h)
	    coroutine.yield()
	    res[k] = test
	 end
	 sim.stop()
	 json.save('/tmp/data-'..timestamp()..'.lua', res)
      end
   )
   
   sim.every(duration, co)

end


local function sum(a, b)
   if not a then return b 
   else 
      local res = {}
      
      for k,v in ipairs(a) do
	 local rhs = 0
	 if b[k] then rhs = b[k] end
	 res[k] = a[k] + rhs
      end
      return res
   end
end

local function scal(lambda, b)
   local res = {}
   
   for k,v in ipairs(b) do
      res[k] = lambda * b[k]
   end
   return res;
end


function linear.average( data )
   
   local res = {}
   
   local count = 0
   for name, stuff in pairs(data) do
      local avg = plot.new()
      
      for time, p in pairs(stuff) do
	 
	 if #avg.plot == 0 then 
	    avg.plot = p.plot
	    avg.set = p.set
	    avg.unset = p.unset
	 end

	 for k, d in ipairs(p.data) do
	    avg.data[k] = sum(avg.data[k], d)
	 end
	 count = count + 1
      end

      -- normalize by samples count
      for time, values in pairs(avg.data) do
	 avg.data[time] = scal(1/count, avg.data[time])
      end
      
      res[name] = avg
   end
   
   return res
end

key.doc.p = '+/- post-steps'

local post_steps = 2

key.p = function()
	   post_steps = post_steps + 1
	   linear.post( post_steps )
	   text.go('post steps:'..post_steps)
	end

key.P = function()
	   post_steps = post_steps - 1
	   if post_steps < 1 then post_steps = 1 end
	   linear.post( post_steps )

	   text.go('post steps:'..post_steps)
	end

local compliance = 0
local compliance_min = 1e-6;

key.doc.c = '+/- compliance (cloth only)'
key.c = function() 
	   
	   if compliance == 0 then compliance = compliance_min 
	   else compliance = compliance * 10 end

	   linear.compliance( compliance )
	   text.go( 'compliance:'.. compliance);

	end

key.C = function() 
	   
	   if compliance <= compliance_min then compliance = 0
	   else compliance = compliance / 10 end
	   
	   linear.compliance( compliance )
	   text.go( 'compliance:'.. compliance);
	end


return linear
