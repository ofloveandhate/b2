import bertini as pb

x = pb.Variable('x')
y = pb.Variable('y')

f = x*y

x.set_current_value(2)
y.set_current_value(3)

print(f.eval_d())