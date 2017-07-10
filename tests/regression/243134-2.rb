def method_missing(*)
  sprintf("%c%s", 0, 0)
  0[]
end

foo
