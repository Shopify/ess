begin
  olddecimal = Decimal.new(1)
  Decimal = Hash
  a = -olddecimal
  raise "#{a}"
rescue RuntimeError
  # no crash
end
