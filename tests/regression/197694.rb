module Test
end

def method_missing(s)
  #puts 'method_missing'
  extend(Test)
  #puts 'method_missing done'
end

def set
  #puts 'set'
  a.set
  #puts 'set done'
end

set
