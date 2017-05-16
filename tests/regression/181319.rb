10.times do
  @a = []
  N = 1000
  $a = ("a"*0xf + "\n") * N

  $a.lines do |l|
    $a.clear
    foo = "UUUUUUUU" * 1000
    @a << l
  end

  unless @a == (["a"*0xf + "\n"] * N) || @a == ["a"*0xf + "\n"]
    raise "regression test failed"
  end
end
