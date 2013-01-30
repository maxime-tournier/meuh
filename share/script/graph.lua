local graph = {}


function graph.viz(source, dest)
   assert( dest )

   local _, _, ext = string.find(dest, '([^\.]+)$')
   local cmd = 'neato -T'..ext..' -o'..dest..' '..source

   os.execute(cmd)
end


function graph.view(source)

   local dest = string.gsub(source, '[^\.]+$', 'jpg')
   print( 'writing output to', dest)
   
   graph.viz(source, dest)
   os.execute('xdg-open '..dest)
end


return graph