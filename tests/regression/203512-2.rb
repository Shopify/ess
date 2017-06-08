begin
  [0].each do
    [].each 0
  end
rescue=>e
end
GC.start
puts e.backtrace
