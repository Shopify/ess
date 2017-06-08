class A
  def foo
  end
end

class B < A
  def foo(*args)
    super(*args, &:b)
  end
end

B.new.foo
