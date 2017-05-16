module EnterpriseScriptService
  module Protocol
    class << self
      def packer_factory
        @packer_factory ||= begin
          factory = MessagePack::Factory.new
          factory.register_type(0x00, Symbol)
          factory
        end
      end
    end
  end
end
