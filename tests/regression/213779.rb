begin
rescue => a
end

begin
  b
rescue begin
    c ""
  rescue => d
    0
  ensure
  end
end
