begin
  class Class
    attr_reader :new
  end

  Class.new.foo
rescue TypeError
  # no crash
end
