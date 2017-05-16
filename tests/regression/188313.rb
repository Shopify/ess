begin
  NilClass = nil
  GC.enable
  GC.start
  Array.dup
  nil.size
rescue NoMethodError
  # no crash
end
