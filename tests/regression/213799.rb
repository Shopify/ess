def f(k)
    H ||= Module.new { f(0) }
end

f(0)
