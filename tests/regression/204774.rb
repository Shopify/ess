def a
  b
ensure
  proc { return }.call
end

begin
  a
rescue
end
