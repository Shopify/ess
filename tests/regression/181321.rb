begin
  class A
    def to_ary
      $a.clear
      nil
    end
  end
  $a = [A.new]
  $a.to_h
rescue TypeError
  # no crash
end
