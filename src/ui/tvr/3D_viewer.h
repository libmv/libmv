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

#ifndef UI_TVR_3D_VIEWER_H_
#define UI_TVR_3D_VIEWER_H_

#include <QGLWidget>
#include <vector>
#include <utility> //For std::pair.

#include "ui/tvr/tvr_document.h"
#include "libmv/scene_graph/scene_graph.h"

class SceneObject {
 public:
  // The drawing function should assume that
  // opengl's model matrix is already ready for drawing.
  // Changing the model matrix won't affect children.
  virtual void Draw() {};
};

class SceneCamera : public SceneObject {
 public:
  // Position, scale and rotation are stored in the SGNode Matrix,
  // so SceneCamera only needs a drawing function.
  void Draw();
};

class ScenePointCloud : public SceneObject {
 public:
  void Draw();
  void AddPoint(libmv::Vec3 &);
 private:
  // Feel free to change this to a list if removal of points is needed (Daniel).
  std::vector<libmv::Vec3> points_;
};

class SceneImage : public SceneObject {
 public:
  void Draw();
  SceneImage(GLuint texture) : texture_(texture) {}
 private:
  GLuint texture_;
};

// A widget displaying a 3D scene, including:
//  - Cameras.
//  - Point clouds.
//  - Images.
class Viewer3D : public QGLWidget {
  Q_OBJECT
  
 public:
  Viewer3D(QWidget *parent = 0);
  ~Viewer3D() {};

  QSize minimumSizeHint() const {return QSize(0,0);};
  QSize sizeHint() const {return QSize(0,0);};

 public slots:
  void SetDocument(TvrDocument *);
  void UpdateScreenImage(int) {};
  void SetTransformation(float, float, float) {};

 protected:
  // Drawing.
  void initializeGL() {};
  void paintGL() {};
  void resizeGL(int, int) {};
  void SetUpGlCamera() {};
  void DrawImage(int) {};
  void DrawFeatures(int) {};
  void DrawCandidateMatches() {};

  // Mouse.
  void mousePressEvent(QMouseEvent *) {};
  void mouseMoveEvent(QMouseEvent *) {};
  void wheelEvent(QWheelEvent *) {};
  int ImageUnderPointer(QMouseEvent *) {return 0;};

  // Coordinate systems.
  void PlaneFromScreen(float, float, float *, float *) {};
  void ScreenFromPlane(float, float, float *, float *) {};

 private:
  TvrDocument *document_;
  libmv::SceneGraph<SceneObject> scene_graph;
};

#endif // UI_TVR_3D_VIEWER_H_
