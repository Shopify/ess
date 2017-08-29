1000.times { [0].zip [0] }
(0..1000).to_a
ObjectSpace.each_object{ |obj| obj[] rescue 0 }
