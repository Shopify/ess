  DELEGATE = [:puts, :p]

·  def method_missing(name, *args, &block)
    super unless DELEGATE.include? na‎me
    ::Kernel.send
end
