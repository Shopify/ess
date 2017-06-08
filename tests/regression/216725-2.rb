h = {}
a = []
20.times do |n|
  a[0] = n
  h[a] = 0
end
h.dup.keys
