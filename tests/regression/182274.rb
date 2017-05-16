$x = Time.new
class Tmp
    def to_i
        $x.mday
    end
end
$x.initialize Tmp.new
