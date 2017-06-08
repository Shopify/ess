class A < def to_str
  ""[1, 2, 3]
  ensure --> {} rescue
  Struct.new.new.to_h
  end
end
