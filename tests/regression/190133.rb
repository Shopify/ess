begin
  [].values_at true
rescue TypeError
  # no crash
end

begin
  Struct.new.new.values_at true
rescue TypeError
  # no crash
end
