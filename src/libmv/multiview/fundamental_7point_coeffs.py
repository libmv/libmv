import sympy

F1 = sympy.Matrix([sympy.symbols('abcdefghi')]).reshape(3,3)
F2 = sympy.Matrix([sympy.symbols('jklmnopqr')]).reshape(3,3)
z = sympy.Symbol('z')

equation = (F1 + z*F2).det()  # == 0
d = equation.as_poly(z).as_dict()

print '  // Coefficients in ascending powers of alpha, i.e. P[N]*x^N.'
print '  double P[4] = {'
for i in range(4):
    print '    %s,' % d[(i,)]
print '  }'
