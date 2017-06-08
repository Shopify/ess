def g
  raise "test"
end

def g
  def f
    begin
      g
    rescue => e
      raise e
    end
  end

  a f
  begin
    g
  rescue => e
    raise e
  end
end

begin
  g
rescue => e
  raise e
end

a f
begin
  g
rescue => e
  raise e
end

a f
begin
  g
rescue => e
  raise e
end
