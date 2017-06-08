def one
  too { yield }
end

def too
  yield
ensure
  one { break }
end

one
