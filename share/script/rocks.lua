

-- simple luarocks wrapper

if not pcall(require, 'luarocks.loader') then
   error('you need to install luarocks first, e.g. using: sudo apt-get install luarocks')
end

local res = {}

-- auto-handles rocks installation
function res.require( module, rockname ) 
   
   rockname = rockname or module
   
   local res = pcall(require, module )
   if not res then 
      os.execute('luarocks --local install '..rockname)
   end
   
   return require(module)
end


return res

