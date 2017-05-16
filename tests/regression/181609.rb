begin
  10.times do
    class A
      def ==(o)
        Struct.new("Test", :a,:b,:c,:d,:e,:f,:g,:h,:i,:j)
        $a.initialize_copy(Struct::Test.new("A","B","C","D","E","F","G","H","I","J"))
        500.times { a = "\x33\x22\x11\x44\x45\x46\x47\x48\x88\x77\x66\x55\x44"*2 }
        true
      end
    end

    Struct.new("Test", :a,:b,:c,:d,:e)
    $a = Struct::Test.new(A.new, A.new, A.new, A.new, A.new)
    $b = Struct::Test.new("a","b","c","d","e")

    $a == $b
  end
rescue TypeError
  # no crash
end
