def a
  yield
ensure
  GC.start
end
+lambda { a { a { a { return proc{} } } } }.call
