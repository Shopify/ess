def a
  b { yield }
end

def b
  Fiber.new { yield }.resume
ensure
  a { break }
end

a
