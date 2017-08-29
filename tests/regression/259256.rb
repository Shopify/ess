def a
    yield
    rescue *nil
    ensure
    x y = a { return }
end

a { foo a }
