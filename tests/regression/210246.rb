def a
  b
rescue
ensure
  a
  proc { return }.call
end

begin
  a
rescue
end
