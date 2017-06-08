def f(n)
    undef w
ensure
    begin
        f(n-1) if n > 0
    rescue
    end
end
f(15)
