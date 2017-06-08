f = Fiber.new do
    m = Fiber.current
    Fiber.yield Proc.new {}
end

f = f.resume
GC.start
