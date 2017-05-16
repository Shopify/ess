begin
  BasicObject.remove_method(:method_missing)
  1.__send__(:foo)
rescue NoMethodError
  # no crash
end
