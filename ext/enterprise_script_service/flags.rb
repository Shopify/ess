module Flags
  class << self
    def cflags
      debug_flags + optimization_flags
    end

    def debug_flags
      %w(-g3)
    end

    def optimization_flags
      if ENV['MRUBY_ENGINE_ENABLE_DEBUG']
        %w(-O0)
      else
        %w(-O3)
      end
    end

    def io_safe_defines
      %w(
        _GNU_SOURCE
        MRB_ENABLE_DEBUG_HOOK
        MRB_INT64
        MRB_UTF8_STRING
        MRB_WORD_BOXING
        YYDEBUG
      )
    end

    def defines
      io_safe_defines + %w(MRB_DISABLE_STDIO)
    end
  end
end
