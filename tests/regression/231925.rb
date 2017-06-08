a = []
f = Fiber.new do
    x
end
[].call f.resume
g = Fiber.new do end
x = {}
r = RangeError.new x = []
a = Fiber.new do
    r.to_a
end
class Z < a
end
r.resume
