def method_missing(meth,*args)
  yield(meth,args)
end

enum_for.next
