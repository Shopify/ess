class NoMethodError
  def initialize(message, name, args)
    a super &name
  end
end

def a(n)
    Fiber.new{0[]}.resume
end

class A
  f
end
