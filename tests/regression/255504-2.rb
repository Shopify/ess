String(
  def to_str
    "" != []
  ensure
    - -> {}.dup
  end
)
