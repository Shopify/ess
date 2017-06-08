def a(n)
  return 0
ensure
  a(n-1) if n > 0
end

a(200)
