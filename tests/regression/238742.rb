def a
  yield
rescue *nil
ensure
 GC.start { return }
end

a { foo a }
