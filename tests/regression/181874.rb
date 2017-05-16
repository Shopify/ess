begin
  class Child
    remove_method nil
  end
rescue TypeError
  # no crash
end
