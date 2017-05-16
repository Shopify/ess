Range.remove_method(:initialize_copy)
begin
  (1..2).dup.to_s
rescue ArgumentError
  # no crash
end
