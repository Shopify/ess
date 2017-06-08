def a
  yield
ensure
  a { return }
end

a { return }
