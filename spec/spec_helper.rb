require("enterprise_script_service")
require("pry")

RSpec.configure do |config|
  config.expect_with(:rspec) do |expectations|
    expectations.include_chain_clauses_in_custom_matcher_descriptions = true
  end

  config.mock_with(:rspec) do |mocks|
    mocks.verify_partial_doubles = true
  end

  config.default_formatter = "doc" if config.files_to_run.one?
  config.disable_monkey_patching!
  config.example_status_persistence_file_path = "spec/examples.txt"
  config.filter_run_when_matching(:focus)
  config.order = :random
  config.warnings = true

  Kernel.srand(config.seed)
end
