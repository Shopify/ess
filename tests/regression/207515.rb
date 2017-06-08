def method_missing(sym, *args, &blk)
   !@original.send(sym, *args, -blk)
end

puts "not.nil? => #{nil.not.nil?}"
