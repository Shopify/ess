module EnterpriseScriptService
  Result = Struct.new(:output, :stdout, :stat, :measurements, :errors) do
    def initialize(output:, stdout:, stat:, measurements:, errors:)
      super(output, stdout, stat, measurements, errors)
    end

    def success?
      errors.empty?
    end
  end
end
