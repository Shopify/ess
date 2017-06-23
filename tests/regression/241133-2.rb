def aa(n)
  if n >= 0
    instance_exec(x)
  end
rescue
  aa(n-1)
end

x = GC.interval_ratio = 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
2.times{x.to_s}
aa(200)
y = Class.new.dup
1.times{y.to_s}
