def method_missing(m, *)
  a self.b ||= 0
end

c
