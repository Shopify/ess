b = []
c = 0

class A < Fiber
end

x = A.new {a}.resume
y = A.new {a}.resume
m = A.new {}.resume

d = b.to_s
Array.new {}

begin
    break
    n = A.new {}
    z = A.new {}
    ensure
    z.resume
end
