$s = "01234\n"
class Tmp
  def to_i
    $s.chomp!
    95
  end
end
$tmp = Tmp.new
begin
  $s.setbyte(5, $tmp)
  raise "regression test failed"
rescue IndexError
  # issue is fixed
end
