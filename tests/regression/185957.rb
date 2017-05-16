class AssertionError < StandardError
end

def assert_raise(type)
  begin
    yield
    raise AssertionError, "Didn't raise #{type}"
  rescue type
  end
end

class << Object
  alias_method :new_, :new
end

[Symbol, Integer, Float, NilClass, TrueClass, FalseClass].each do |klass|
  assert_raise(NoMethodError) { klass.new }
  assert_raise(TypeError) { klass.new_ }
end
