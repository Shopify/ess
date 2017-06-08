def a 
    Fiber.new{yield}.resume 
end
1.times { lambda{ a {  return } } .call }
