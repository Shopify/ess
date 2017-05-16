begin
  def nil.b
    b *nil
  end
  nil.b
rescue SystemStackError
  # no crash
end
