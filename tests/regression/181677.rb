begin
  b = a () ? 1 : 0
rescue NoMethodError
  # no crash
end
