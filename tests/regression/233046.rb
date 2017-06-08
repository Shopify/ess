class NoMethodError
  def initialize(*)
  end

  ObjectSpace.each_object{ |obj| obj===[] }
end
