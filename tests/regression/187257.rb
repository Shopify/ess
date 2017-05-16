begin
  Object.undef_method 123
rescue TypeError
  # no crash
end
