def aa(n)
  if n >= 0
    instance_exec(x)
  end
rescue
    aa (n-1)
end

def x
yield
ensure
format 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
end


GC.start

g = Fiber.new do
x {yield}
end
begin
  g.resume
rescue
end
aa(100)
