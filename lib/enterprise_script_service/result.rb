module EnterpriseScriptService
  Result = Struct.new(:output, :stdout, :stat, :measurements, :errors, keyword_init: true) do
    def success?
      errors.empty?
    end
  end
end
