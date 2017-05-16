begin
  old_type_error = TypeError
  TypeError = nil
  123 - nil
rescue old_type_error
  # no crash
end
