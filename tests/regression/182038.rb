def create_lambda
  lambda {|x| local_variables}
end
create_lambda.call(nil)
