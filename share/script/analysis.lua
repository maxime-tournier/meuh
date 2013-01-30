


require 'serialize'


file = '/home/max/tmp/results.lua'



full = io.load(file)
if not full then full = {} end

function update() 
   full[ app().modes() ] = result
   io.save(file, full)
end



function sorted_keys(x)
   local res = {}
   for key, _ in pairs(x) do
      table.insert(res, key)
   end

   table.sort( res )
   return res
end

function sorted_table(x)
   local keys = sorted_keys(x)

   local res = {}

   for _, key in pairs(keys) do
      res[key] = x[key]
   end
   
   return res
end

plot_file = '/home/max/tmp/plot.dat'

function save_plot( where )
   if not where then where = plot_file end
   
   io.output( where )
   
   local sorted = sorted_table( full )

   for key, value in pairs(sorted) do
      
      k, v = next(value)

      local samples = v.m * v.n
      local deg = math.pi / 180
      
      local pga_rms = math.sqrt(v.pga / samples) / deg;
      local euler_rms = math.sqrt(v.euler / samples) /deg ;
      
      io.write(key, ' ', pga_rms, ' ', euler_rms, '\n')
      
   end
   io.close()
end

function show_plot( what )
   if not what then what = plot_file end

   cmd = [[
	 gnuplot -e '
	 set xlabel "Geodésiques";
	 set ylabel "RMS orientation, degrés";
	 plot "filename" using 1:3 with lines title "Euler" linewidth 3, 
	 "filename" using 1:2 with lines title "PGA" linewidth 3 lt 3; 
	 pause - 1'
   ]]
   cmd = string.gsub(cmd, 'filename', what)
   
   os.execute(cmd)
   
end

if result then 
   update()
else
   save_plot()
   show_plot()
end