f = Fiber.new do
end
GC.start

def a(n)
  if n >= 0
    instance_exec(x)
  end
rescue
    a (n-1)
end
a (10)
f.initialize{break}
a (100)
f.resume
