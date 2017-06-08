class A
  def foo(*args)
  end
end

class B < A
  def foo(*args)
    super(*args, &:bar)
  end

  # It doesn't matter if this method is defined or not
  def bar
  end
end

B.new.foo
