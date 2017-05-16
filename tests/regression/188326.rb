100000.times do
  begin
    Time.at(0xd00000000000000).to_s
  rescue ArgumentError
    # no crash
  end
end
