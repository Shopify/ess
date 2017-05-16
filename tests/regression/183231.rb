begin
  0[<<AAA,
  #{<<BBB}
BBB
AAA
  0]
rescue NoMethodError
  # no crash
end
