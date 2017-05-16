class Decimal
  include Comparable

  ZERO = Decimal.new(0)
end

class Fixnum
  def to_d
    Decimal.new(self)
  end
end

class Float
  def to_d
    Decimal.new(self)
  end
end

class String
  def to_d
    Decimal.new(self)
  end
end
