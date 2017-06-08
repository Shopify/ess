Array.new{{}}
begin
    Fiber.new{+ x}.resume
rescue
end

{}0
x.a
