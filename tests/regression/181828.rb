begin
  def method
      yield
  end

  method(&a &&= 0)
rescue LocalJumpError
  # no crash
end
