def method_missing(m)
ensure
begin A rescue
break
rescue
end
end

send ''
