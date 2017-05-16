begin
  NoMethodError.define_singleton_method(:new) do "waat" end
  Object.q
rescue TypeError
  # no crash
end
