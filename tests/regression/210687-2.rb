BasicObject.remove_method(:method_missing)

class A
def foo
super
end
end

A.new.foo
