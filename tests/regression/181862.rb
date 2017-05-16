begin
  members = []
  B = Struct.new("B", members)
  s2 = B.new
  members << :b
  members << :c
  members << :d
  members << :e
  s2.to_h
rescue TypeError
  # no crash
end
