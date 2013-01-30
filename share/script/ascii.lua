-- simple ascii file loader

local res = {}

local function open( filename, mode )
   return assert(io.open(filename, mode))
end

function res.load( filename ) 
    local f = open( filename , 'r' )
    
    local res = f:read("*all")
    f:close()
    
    return res
 end


function res.save( filename, str )
   local f = open( filename, 'w' )
   f:write( str )
   f:close()
end

return res