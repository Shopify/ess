def f(k)
    Struct.new {n rescue f(0)}
end

f(0)
