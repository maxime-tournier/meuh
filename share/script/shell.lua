local shell = {}

function shell.exec( cmd ) 
   -- to execute and capture the output, use io.popen
   local f = io.popen(cmd) -- store the output in a "file"
   local res =  f:read("*a")    -- print out the "file"'s content
   f:close()

   -- chomp result
   return string.gsub(res, "[\r\n]+$", "")
end



return shell

