// Copyright (c) 2009 libmv authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Generic Image Processing Algorithm (GIPA)
// Use an ImageModel class that must implement the following :
//
// ::Contains(int y, int x) <= Tell if a point is inside or not the image
// ::operator(int y,int x)  <= Modification accessor over the pixel (y,x)

namespace libmv {

// Bresenham approach to draw ellipse.
// http://raphaello.univ-fcomte.fr/ig/algorithme/ExemplesGLUt/BresenhamEllipse.htm
// Add the rotation of the ellipse.
// As the algo. use symmetry we must use 4 rotations.
template <class Image, class Color>
void DrawEllipse(int xc, int yc, int radiusA, int radiusB,
                 const Color &col, Image *pim, double angle = 0.0)
{
  int a = radiusA;
  int b = radiusB;
  Image &im = *pim;

  // Counter Clockwise rotation matrix.
  double matXY[4] = { cos(angle), sin(angle), 
                     -sin(angle), cos(angle)};
  int x,y;
  double d1,d2;
  x = 0;
  y = b;
  d1 = b*b - a*a*b + a*a/4;
  
  float rotX = (matXY[0] * x + matXY[1] * y);
  float rotY = (matXY[2] * x + matXY[3] * y);
  if (im.Contains( yc + rotY,  xc + rotX))
    im( yc + rotY,  xc + rotX) = col;
  rotX = (matXY[0] * x - matXY[1] * y);
  rotY = (matXY[2] * x - matXY[3] * y);
  if (im.Contains( yc + rotY,  xc + rotX))
    im( yc + rotY,  xc + rotX) = col;
  rotX = (-matXY[0] * x - matXY[1] * y);
  rotY = (-matXY[2] * x - matXY[3] * y);
  if (im.Contains( yc + rotY,  xc + rotX))
    im( yc + rotY,  xc + rotX) = col;
  rotX = (-matXY[0] * x + matXY[1] * y);
  rotY = (-matXY[2] * x + matXY[3] * y);
  if (im.Contains( yc + rotY,  xc + rotX))
    im( yc + rotY,  xc + rotX) = col;
  while ( a*a*(y-.5) > b*b*(x+1) ) {
    if ( d1 < 0 ) {
      d1 += b*b*(2*x+3);
      ++x;
    }
    else
    {
      d1 += b*b*(2*x+3) + a*a*(-2*y+2);
      ++x;
      --y;
    }
    rotX = (matXY[0] * x + matXY[1] * y);
    rotY = (matXY[2] * x + matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
      im( yc + rotY,  xc + rotX) = col;
    rotX = (matXY[0] * x - matXY[1] * y);
    rotY = (matXY[2] * x - matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
        im( yc + rotY,  xc + rotX) = col;
    rotX = (-matXY[0] * x - matXY[1] * y);
    rotY = (-matXY[2] * x - matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
       im( yc + rotY,  xc + rotX) = col;
    rotX = (-matXY[0] * x + matXY[1] * y);
    rotY = (-matXY[2] * x + matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
      im( yc + rotY,  xc + rotX) = col;
  }
  d2 = b*b*(x+.5)*(x+.5) + a*a*(y-1)*(y-1) - a*a*b*b;
  while ( y > 0 ) {
    if ( d2 < 0 ) {
      d2 += b*b*(2*x+2) + a*a*(-2*y+3);
      --y;
      ++x;
    }
    else {
      d2 += a*a*(-2*y+3);
      --y;
    }
    rotX = (matXY[0] * x + matXY[1] * y);
    rotY = (matXY[2] * x + matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
      im( yc + rotY,  xc + rotX) = col;
    rotX = (matXY[0] * x - matXY[1] * y);
    rotY = (matXY[2] * x - matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
      im( yc + rotY,  xc + rotX) = col;
    rotX = (-matXY[0] * x - matXY[1] * y);
    rotY = (-matXY[2] * x - matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
      im( yc + rotY,  xc + rotX) = col;
    rotX = (-matXY[0] * x + matXY[1] * y);
    rotY = (-matXY[2] * x + matXY[3] * y);
    if (im.Contains( yc + rotY,  xc + rotX))
      im( yc + rotY,  xc + rotX) = col;
  }
}

// Bresenham approach do not allow to draw concentric circle without holes.
// So it's better the use the Andres method.
// http://fr.wikipedia.org/wiki/Algorithme_de_tracé_de_cercle_d'Andres.
template <class Image, class Color>
void DrawCircle(int x, int y, int radius, const Color &col, Image *pim) {
  Image &im = *pim;
  if (  im.Contains(y + radius, x + radius)
     && im.Contains(y + radius, x - radius)
     && im.Contains(y - radius, x + radius)
     && im.Contains(y - radius, x - radius)) {
    int x1 = 0;
    int y1 = radius;
    int d = radius - 1;
    while (y1 >= x1) {
      // Draw the point for each octant.
      im( y1 + y,  x1 + x) = col;
      im( x1 + y,  y1 + x) = col;
      im( y1 + y, -x1 + x) = col;
      im( x1 + y, -y1 + x) = col;
      im(-y1 + y,  x1 + x) = col;
      im(-x1 + y,  y1 + x) = col;
      im(-y1 + y, -x1 + x) = col;
      im(-x1 + y, -y1 + x) = col;
      if (d >= 2 * x1) {
        d = d - 2 * x1 - 1;
        x1 += 1;
      } else {
        if (d <= 2 * (radius - y1)) {
          d = d + 2 * y1 - 1;
          y1 -= 1;
        } else  {
          d = d + 2 * (y1 - x1 - 1);
          y1 -= 1;
          x1 += 1;
        }
      }
    }
  }
}

// Bresenham algorithm
template <class Image, class Color>
void DrawLine(int xa, int ya, int xb, int yb, const Color &col, Image *pim) {
  Image &im = *pim;
  // TODO(pau): Deal with start and end points outside the image.
  if (!im.Contains(ya, xa) && !im.Contains(yb, xb))
    return;

  int xbas, xhaut, ybas, yhaut;
  // Check the condition ybas < yhaut.
  if (ya <= yb) {
    xbas = xa;
    ybas = ya;
    xhaut = xb;
    yhaut = yb;
  } else {
    xbas = xb;
    ybas = yb;
    xhaut = xa;
    yhaut = ya;
  }

  // Initialize slope.
  int x, y, dx, dy, incrmX, incrmY, dp, N, S;
  dx = xhaut - xbas;
  dy = yhaut - ybas;
  if (dx > 0) { // If xhaut > xbas we will increment X.
    incrmX = 1;
  } else {
    incrmX = -1; // else we will decrement X.
    dx *= -1;
  }
  if (dy > 0) { // Positive slope will increment X.
    incrmY = 1;
  } else {      // Negative slope.
    incrmY = -1;
  }
  if (dx >= dy) {
    dp = 2 * dy - dx;
    S = 2 * dy;
    N = 2 * (dy - dx);
    y = ybas;
    x = xbas;
    while (x != xhaut) {
      im(y,x) = col;
      x += incrmX;
      if (dp <= 0) { // Go in direction of the South Pixel.
        dp += S;
      } else {       // Go to the North.
        dp += N;
        y+=incrmY;
      }
    }
  } else {
    dp = 2 * dx - dy;
    S = 2 * dx;
    N = 2 * (dx - dy);
    x = xbas;
    y = ybas;
    while (y < yhaut) {
      im(y,x) = col;
      y += incrmY;
      if (dp <= 0) { // Go in direction of the South Pixel.
        dp += S;
      } else {       // Go to the North.
        dp += N;
        x += incrmX;
      }
    }
  }
  im(y,x) = col;
}

} //namespace libmv
