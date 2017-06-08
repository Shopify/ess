def a
  yield
ensure
  GC.start
  lambda { return }.call
end
a { return }.call
