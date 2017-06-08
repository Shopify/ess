h=Hash.new{|h,k|break h[k] = k*0}
h.default(0)
