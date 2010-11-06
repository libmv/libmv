// Copyright (c) 2010 libmv authors.
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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#include "libmv/base/vector.h"
#include "libmv/numeric/numeric.h" 

class GLWidget : public QGLWidget
{     
 public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget();
       
  void AddNewStructure(libmv::vector<libmv::Vec3> &struct_coords); 
  
 protected:
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();    
  
  void DrawPointStructure(libmv::Vec3 &p);
  
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent (QMouseEvent *event);
  void wheelEvent     (QWheelEvent *event);
  
 private:
  // Contains a list of structures
  QList<GLuint> structures_list_;
  QPoint        last_mouse_position_;    
  libmv::Vec3f  viewer_orientation_;
  libmv::Vec3f  viewer_position_;
};

#endif // GLWIDGET_H
