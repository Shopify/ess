def to_str
  @b + + -> {}
rescue to_str
  @b + + -> {}
rescue ""
end

class A < to_str
end
