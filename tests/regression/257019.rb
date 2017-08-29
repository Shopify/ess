def to_str
    class<<Array
        foo class<<Array
            $a|@@a||= Math
        end
    end
end.a@@a
