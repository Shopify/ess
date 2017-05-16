begin
  NotImplementedError = String
  Module.constants
rescue RuntimeError, TypeError
  # no crash
end
