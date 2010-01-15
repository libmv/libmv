# Minimal Solutions for Panoramic Stitching. M. Brown, R. Hartley and D. Nister. 
# International Conference on Computer Vision and Pattern Recognition
# (CVPR2007). Minneapolis, June 2007.

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

# As long as Python code do not return the correct value
# I prefer use "Xcas Computer Algebra System" 
# http://www-fourier.ujf-grenoble.fr/~parisse/giac.html

# Solution for the focal length of a pair of images from a rotating camera.
# comment(" Define the base equation that share the Focal length 'f' ")
# FocalSolver:=(a12+f)^2 / (( a1+f)*(a2+f)) - (b12+f)^2 / ((b1+f)*(b2+f));
# comment(" Solve equation=0 with the unknow 'f' ")
# solve(FocalSolver=0,f);
