NilClass.remove_method :to_s
nil.to_s

Symbol.remove_method :to_s
(:a).to_s

Fixnum.remove_method :to_s
1.to_s
