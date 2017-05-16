class A
  def ==(*)
    $a.clear
    false
  end
end

$a = [nil, A.new]
$a.rindex(nil)
