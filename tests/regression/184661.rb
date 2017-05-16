begin
  class Time
    def initialize
    end
  end

  a = Time.new
  b = Time.new
  a.initialize_copy b
rescue ArgumentError
  # no crash
end
