f = Fiber.new do
  f = Fiber.new do
  end
end

f.resume
GC.start

f.resume
