1.times {}
688.times { [0].zip [0] }
169.times { [0].zip  }
ObjectSpace.each_object{|obj| obj[] rescue 0}
