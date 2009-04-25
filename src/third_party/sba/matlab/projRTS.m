function pt=projRTS(rt, xyz, a)
% symbolic projection function
  a1=a(1); a2=a(2); a3=a(3); a4=a(4); a5=a(5);
  qr2=rt(1); qr3=rt(2); qr4=rt(3);
  t1=rt(4); t2=rt(5); t3=rt(6);
  X=xyz(1); Y=xyz(2); Z=xyz(3);

  qr1=sqrt(1-(qr2^2+qr3^2+qr4^2));

  pt=[-((t3+Z+(-2*qr1*X+2*qr4*Y-2*qr3*Z)*qr3+(2*qr1*Y+2*qr4*X-2*qr2*Z)*qr2)*a3+(t2+Y+(2*qr1*X-2*qr4*Y)*qr4+2*qr3*qr4*Z+(2*qr3*X-2*qr1*Z-2*qr2*Y)*qr2)*a2+(X+t1+(-2*qr4*X-2*qr1*Y)*qr4+(-2*qr3*X+2*qr1*Z)*qr3+(2*qr3*Y+2*qr4*Z)*qr2)*a1)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2),...
  ((-Z-t3)*a5+(-2*a5*qr4*Y+2*a5*qr1*X+2*a5*Z*qr3)*qr3+(-2*a5*qr1*Y-2*a5*qr4*X+2*a5*Z*qr2)*qr2+(-t2-Y+(-2*qr1*X+2*qr4*Y)*qr4-2*qr3*qr4*Z+(-2*qr3*X+2*qr1*Z+2*qr2*Y)*qr2)*a4)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)];
