a = Proc.new do
end

class Proc
  def initialize
  end
end

b = Proc.new
a.initialize_copy b
