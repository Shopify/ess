def f(n)
  return 0
ensure
  f(n-1) if n > 0
end

f(200)
