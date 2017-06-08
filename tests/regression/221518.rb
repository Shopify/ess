begin
  [0].each do
    ['00'].each 0
  end
rescue=>e
end
GC.start
e.backtrace
