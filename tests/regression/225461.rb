f = Fiber.new do
1000.times { [0].zip [0] }
end
f = f.resume
