def a
  yield
ensure
  Array.new.uniq {}
end

def b
  yield
ensure
  lambda { a { a a { return proc{} } } }.call
end

GC.start

lambda { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { b { return proc{} } } } } } } } } } } } } } } } } } } } } } } }.call
