def a
  yield
ensure
lambda { a { return } }.call
end

lambda { a { return } }.call
