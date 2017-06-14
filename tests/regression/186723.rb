begin
  old_no_method_error = NoMethodError
  NoMethodError = Fixnum
  boom!
rescue old_no_method_error
  # no crash
end
