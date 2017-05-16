begin
  Range = Array
  (1..2).inspect
rescue TypeError
  # no crash
end
