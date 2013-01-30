local api = require 'api'


local linear = api.load('linear')

local sim = require 'simulator'

local key = require 'key'
local text = require 'text'
local viewer = require 'viewer'
local pretty = require 'pretty'

local log = require 'log'
local plot = require 'plot'
local shell = require 'shell'

require 'serialize'

require 'sorted'


local graph = 1
local precision = -2


function linear.show(val)
   linear.show_graph:set(toboolean(val))
end

function linear.solver(val)
   local cholesky = 0
   local cg = 1
   
   if val then 
      linear.solver_kind:set( cholesky ) 
      text.go("Cholesky")
   else 
      linear.solver_kind:set( cg )
      text.go("CG")
   end
end

-- TODO description ?
local graph_name = { "DFS", "MST", "PROP", "RND", "PGS",  "CG", "NONE", "CMK" }

linear.graph_name = graph_name

local old = {}
local old_graph = linear.graph

function linear.graph(val)
   local tmp = ((val - 1) % #graph_name) + 1
   
   old_graph( tmp ) 
   text.go( graph_name[tmp] )
   
end



key.d = key.toggle(linear.show, false)
key.doc.d = 'toggles graph drawing'

key.x = function ()
	   graph = graph + 1
	   linear.graph( graph )
	end
key.doc.x = 'cycles graph type'

key.X = function ()
	   graph = graph - 1
	   linear.graph( graph )
	end


local function add(a, b) 
   for k,_ in pairs(b) do
      a[k] = a[k] or 0
      a[k] = a[k] + b[k]
   end
end

local function div(a, b) 
    for k,_ in pairs(a) do
       a[k] = a[k] / b
   end
end

-- function linear.process( results, data, by )
   
--    local time = 0.0
--    local n = 0

--    local solve = { iter = 0, time = 0, error = 0}
   
--    local graph = { time = 0}
   
--    local first = nil
   
--    for t,chunk in pairs(data) do
      
--       first = first or t 
      
--       add( solve, chunk.solve )
--       add(graph, chunk.graph )
--       time = time + chunk.time
      
--       n = n + 1
--    end

   
--    div( solve, n )
--    div( graph, n )
--    time = time / n

--    local ref = data[ first ]
--    local key = ref.cg.target


--    local key = solve.iter;
--    results[ key ] = results[key] or {} 
   
--    results[ key ][ ref.graph.type ] = { solve = solve, graph = graph, time = time }
   
--    print('iterations:', solve.iter, "time:", time, "error:", error)
--    print()
   
-- end





local function setup(graph, eps, iter)
   linear.eps( eps )
   linear.iter( iter )
   linear.graph( graph )

   print('graph:', graph_name[graph], 'eps:', eps, 'iter:', iter)

   sim.reset()
end



local function key2str(k1, k2)
   local res = ''
   res = res..k1..':'..k2
   return res
end

local function str2key( s )
   
   local _, _, first, second = string.find(s, '([^:]+):([^:]+)' )
   return tonumber(first), tonumber(second)
   
end


function linear.by_iter(from, to)

   from = from or 1
   to = to or 50

   return coroutine.create( function() 
				   for iter = from, to do
				      
				      for graph = 1,#graph_name do
					 setup(graph, 0, iter)
					 coroutine.yield( key2str(graph,iter) )
				      end
				      
				   end
				   
				end )
end


function linear.by_error(from, to)
   
   from = from or 1
   to = to or 10
 
   return coroutine.create( function() 
			       for precision = from, to do
				  local eps = 10^ -precision
				  
				  for graph = 4,#graph_name do
				     setup(graph, eps, -1)
				     coroutine.yield( key2str(graph, eps))
				  end
				  
			       end
			    end )
end


-- averages logged quantities over the whole simulation for each data
-- key
function linear.average(data) 
   
   local res = {}
   
   for k,v in pairs(data) do
      
      local graph, key = str2key(k)
      
      res[key] = res[key] or {} 
      res[key][graph] = res[key][graph] or {}
      
      local r = res[key][graph]
      r.solve = { time = 0, iter = 0, error = 0 }
      r.time = 0

      local n = 0
      
      for t, chunk in pairs( v ) do
	 add(r.solve, chunk.solve)
	 r.time  = r.time + chunk.time
	 
	 n = n + 1
      end

      div( r.solve, n )
      r.time = r.time / n
   end

   return res
end



function linear.plot( data, what  )
   
   local p = plot.new()

   local index = {}

   local plots = 0
   
   for key, results in pairs( data ) do

      p.data[key] = {}
      
      for graph, res in sorted_pairs(results) do
	 
	 if not index[graph] then
	    plots = plots + 1
	    index[graph] = plots
	 end
	 
	 p.data[key][graph] = what( res )
      end
   end
   
   for g,i in sorted_pairs(index) do
      p.plot['1:'..(i+1)] = { title = string.format('%q', graph_name[g]), with = 'lines' }
   end
   
   -- plot.show( p )
   return p
end

  
local solve = {}
function solve.iter( res ) 
   return res.solve.iter
end

function solve.error( res ) 
   return res.solve.error
end

function solve.time( res ) 
   return res.solve.time
end

local graph = {}
function graph.time( res ) 
   return res.graph.time
end



key.doc.w = 'changes cloth width'
key.w = function() 
	   local w = linear.width:get() - 1
	   if w < 2 then w = 2 end
	   linear.dim( w, w )
	end

key.W = function() 
	   local w = linear.width:get() + 1
	   linear.dim( w, w )
	end




local old_eps = linear.eps;

function linear.eps( eps )
   old_eps(eps)
   text.go("eps: "..eps)
end

key.doc.e = 'changes solver precision'
key.e = function()
	   precision = precision - 1
	   linear.eps( 10 ^ precision )
	end

key.E = function()
	   precision = precision + 1
	   linear.eps( 10 ^ precision )
	end


function linear.info_last() 
   pretty.print( log.data[ sim.t:get() - sim.dt:get() ] )
end


key.doc.i = 'displays info'
key.i = function () 
	   if not sim.post_animate.info then
	      sim.post_animate.info = linear.info_last
	   else 
	      sim.post_animate.info = nil
	   end
	end




linear.post_init = {}

linear.post_init.setup = function()
			    viewer.title("Linear-Time Clothes")
			    viewer.axis( false )

			    linear.show( true )
			    linear.width:set(12)
			    
			    linear.damping( 0 )
			    linear.eps( 1e-7 )
			    
			    local w = linear.width:get();
			    linear.iter( w * w )
			    
			    linear.stiffness( 0 )
			    
			    
			    sim.post_steps( 2 )
			    sim.correct( true )

			    sim.timestep(0.005)
			    
			    linear.eps( 0 )
			    linear.damping( 0 )
			    linear.iter(50)
			    
			    linear.dim(20, 10)
			    linear.graph( 2 )
			    linear.prec( true )
			    
			    -- linear.iter( 2 )
			    
			    -- sim.reset()
			 end

sim.post_reset.setup = function() 

			 end


local function timestamp()
   return shell.exec("date +%F-%T")
end


function linear.plot_error(avg)
   
   local p = linear.plot( avg, solve.iter )
   
   p.set.xlabel = [["Error"]]
   p.set.ylabel = [["Iterations"]]
   
   p.set.logscale = 'x'

   return p
end

function linear.plot_iter(avg)
   
   local p = linear.plot( avg, solve.error )
   
   p.set.xlabel = [["Iterations"]]
   p.set.ylabel = [["Error"]]
   
   p.set.logscale = 'y'

   return p
end


function linear.go_error(duration) 
   
   sim.benchmark(duration, linear.by_error(1, 6), 
		 function( res ) 
		    
		    local avg = linear.average(res.data)
		    io.save('/tmp/'..timestamp()..'_error', avg)

		    plot.show( linear.plot_error(avg) )
		 end )
   
end


function linear.go_iter(duration) 
   
   sim.benchmark(duration, linear.by_iter(1, 3), 
		 function( res ) 
		    
		    local avg = linear.average(res.data)
		    io.save('/tmp/'..timestamp()..'_iter', avg)
		    
		    plot.show( linear.plot_iter(avg) )
		 end )
   
end



key.doc.b = 'benchmark'
key.b = function() linear.go_iter(8) end

key.doc.n = 'shows solver graphs'
key.n = function() 
	   -- plot.show( non_linear )
	   -- plot.show(cg_prec)
	   -- plot.show(cg_unprec)

	   -- plot.show( cg )

	   plot.show( plot.merge(cg_prec, cg_unprec) )
	end


key.doc.p = 'toggles preconditioning'
key.p = key.toggle( function(val) 
		       linear.prec( val )
		       text.go("prec: "..tostring(val))
		    end,
		    true )

old.dim = linear.dim

function linear.dim(w, h)
   old.dim(w, h)
   text.go(tostring(w)..' x '..tostring(h))
end


return linear

