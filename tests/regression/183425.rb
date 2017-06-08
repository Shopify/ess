def foo
end

class X
  alias_method :initialize, :send
end

X.new(:foo)
