class << Kernel
  sym = '`'.to_sym

  alias_method :old_cmd, sym

  define_method(sym) { `test`; x }

  `test`
end

t = `#{sym}`
