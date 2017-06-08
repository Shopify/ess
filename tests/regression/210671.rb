def a
  yield
ensure
  GC.start
  lambda { return }.call
end
lambda { a { a { a { return } } } }.call
