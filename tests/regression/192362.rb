ary = Array.new(1023)
begin
  ary[0x7ffffffffffffc00,0] = Array.new(1024)
rescue ArgumentError
  # no crash
end
