def aa(n)
  if n >= 0
    instance_exec(x)
  end
rescue
    aa (n-1)
end

n = Fiber.new{}.dup
GC.start
n.initialize {break}
aa(100)
n.resume
