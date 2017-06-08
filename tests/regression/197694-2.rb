def recurse(n)
  return if n == 0
  extend(Kernel)
  recurse(n-1)
end

recurse(100)
