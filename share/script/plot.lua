local plot = {}

require 'sorted'

function plot.new( opts ) 

   local res = {}

   res.set = {}
   res.unset = {}
   
   res.plot = {}

   -- affects a *table* for each sample value
   res.data = {}

   res.title = {}
   
   return res
end



local function write_data( p )

   for row, data in sorted_pairs( p.data ) do
      io.write( row, ' ' )
      
      for col, value in sorted_pairs(data) do
	 io.write( value, ' ' )
      end
      io.write( '\n' )
   end

end

local function write_plot( p, datafile )
   
   for name,value in pairs(p.set) do
      
      io.write('set ', name, ' ', value, '\n')
      
   end

   for name,value in pairs(p.unset) do
      
      io.write('unset ', name, ' ', value, '\n')
      
   end
   
   io.write('plot ')
   
   local sep = '';
   
   for using,params in pairs(p.plot) do
      
      io.write(sep, '"', datafile, '" using ', using);
      
      for name,value in pairs(params) do
	 io.write(' ', name, ' ', value )
      end
      
      sep = ', '
   end

   io.write('\n')
   
   io.write('pause -1 \n')

end


function plot.write( p, filename )

   assert( filename )
   local function close()
      if filename then io.close() end
   end
   
   local function open( f )
      if filename then io.output( f ) end
   end
   
   local datafile = nil
   local plotfile = nil
   
   if filename then 
      datafile = filename..'.dat'
      plotfile = filename..'.plt'
   end
   
   open(datafile)
   write_data(p)
   close()

   open(plotfile)
   write_plot(p, datafile)
   close()

end


function plot.show( p, filename  )
   
   filename = filename or '/tmp/plot'

   plot.write( p, filename )
   
   local cmd = [[gnuplot -persist -e 'load %q' & ]]
   
   os.execute( string.format(cmd, filename..'.plt')  )

end






-- function plot.merge(a, b) 
--    local res = plot.new()

--    for x,data in pairs( a.data ) do
--       res.data[x] = {}
      
--       res.data[x][1] = data[1]
      
--       local data_b = b.data[x]

--       if data_b then
-- 	 res.data[x][2] = data_b[1]
--       end
      
--    end

--    -- hardcode !
   
--    res.plot['1:2'] = a.plot['1:2']
--    res.plot['1:3'] = b.plot['1:2']
   
--    res.set = a.set
--    res.unset = a.unset
   
--    return res
-- end


local function quote( what ) 
   return '"'..tostring(what)..'"'
end

local function count( p ) 
   local res = 0
   for k,v in pairs(p) do
      res = res + 1
   end

   return res
end

function plot.push(p, key, value)
   if not p.data[key] then 
      p.data[key] = {}
   end
   
   local at = p.data[key]
  
   local c = count(p.plot)
   if #at == c then

      -- new plot
      local channel = c + 1
      local using = '1:'..(channel + 1)

      local chunk = { with = 'lines' }
      if p.title[ channel ] then 
	 chunk.title = quote( p.title[ channel ] )
      end
      -- print('adding', chunk.title, using)

      p.plot[using] = chunk
   elseif #at < (c - 1) then 
      -- we need to add missing entries in previous plots
      local num = c - 1 - #at

      for i = 1, num do
	 local value = 0
	 if( p.data[ key - 1 ] ) then value = p.data[ key - 1 ][ #at + 1 ] end
	 table.insert(at, value)
      end
   
   end
   
   table.insert(at, value) 
end


return plot