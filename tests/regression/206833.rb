  DELEGATE = [:puts, :p]

  def method_missing(name, *args, &block)
    super unless DELEGATE.include? name
  end
��  def respond_to_missing?(name, include_private = false)
    DELGATE.include?(name) or super
end
