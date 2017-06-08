def f
  Module.new {f}
end
begin
  f
rescue => e
end
e.backtrace
