def a
  yield
rescue y = a { return }
end

a { foo a }
