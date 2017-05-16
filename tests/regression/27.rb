class A
  def to_str
    $s.replace("AA")
    "A"
  end
end

$s = "AAA"
$s.chomp!(A.new)
unless $s == "A"
  raise "regression test failed"
end
