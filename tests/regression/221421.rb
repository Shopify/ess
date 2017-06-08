class CVAR1
def send(c, b, x)
super(*c)
super(*b)
end

end

mvarsend1 = def to_s
yield c==c
end

CVAR1.new.send(mvarsend1, 0, 0)