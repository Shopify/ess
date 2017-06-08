def a(n)
  if n >= 0
    instance_exec(0) {}
    a(n-1)
  end
end

a(1000)
