old_struct = Struct
Struct = String
unless old_struct.new.superclass == old_struct
  raise "regression test failed"
end
