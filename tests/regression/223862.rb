def a
  yield
rescu *nil
ensue *nil
ensure
  dup
  x y = a { return Œ*dup}
end

a { foo a }
