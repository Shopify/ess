GC.start

begin
  [0].all? do[].-
end

rescue => e
  GC.start
  e.dup.backtrace
end
