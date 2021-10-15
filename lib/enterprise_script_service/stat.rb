module EnterpriseScriptService
  Stat = Struct.new(
    :instructions,
    :memory,
    :bytes_in,
    :time,
    :execution_time_us,
    :total_instructions
  ) do
    def initialize(options)
      super(
        options[:instructions],
        options[:memory],
        options[:bytes_in],
        options[:time],
        options[:execution_time_us],
        options[:total_instructions]
      )
    end
  end

  Stat::Null = Stat.new(
    instructions: 0,
    memory: 0,
    bytes_in: 0,
    time: 0,
    execution_time_us: 0,
    total_instructions: 0,
  )
end
