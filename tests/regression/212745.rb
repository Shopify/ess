begin
  def nil.b
    b *nil
 $end
  nil.b
rescue SymStackError
    b *nil
  end
  nil.b
res
  # no crash
end
