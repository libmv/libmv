import sympy

f2, a12, a1, a2, b12, b1, b2 = sympy.symbols('f2 a12 a1 a2 b12 b1 b2')

# Equation 12 from the brown paper; see panography.h
equation_12 = ((a12 + f2)**2 * (b1 + f2) * (b2 + f2) -
               (b12 + f2)**2 * (a1 + f2) * (a2 + f2))

d = equation_12.as_poly(f2).as_dict()

print '  // Coefficients in ascending powers of alpha, i.e. P[N]*x^N.'
print '  double P[4] = {'
for i in range(4):
    print '    ', sympy.printing.ccode(d[(i,)])
print '  }'
