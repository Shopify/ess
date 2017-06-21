def one
  too { yield }
endbegin;1;rescue => e1;e1;end;

def too
  yield
ensure
  one { break }
end

one
