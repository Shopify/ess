def a
  b 0
end

class C
  def inspect
    super @d = nil
    a
  end
end

C.new.inspect
