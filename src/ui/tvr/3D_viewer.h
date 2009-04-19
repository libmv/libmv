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

#include "ui/tvr/tvr_document.h"
#include "libmv/scene_graph/scene_graph.h"
#include "ui/tvr/gl_texture.h"

class SceneObject {
 public:
  virtual ~SceneObject() {};
  
  enum ObjectType {
    Camera, PointCloud, Image
  };
  
  // The drawing function should assume that
  // opengl's model matrix is already ready for drawing.
  // Changing the model matrix won't affect children.
  virtual void Draw() {};
  virtual ObjectType GetType() = 0;
};

class SceneCamera : public SceneObject {
 public:
  // Position, scale and rotation are stored in the SGNode Matrix,
  // so SceneCamera only needs a drawing function.
  void Draw();
  ObjectType GetType() { return Camera; }
  
  ~SceneCamera() {};
};

class ScenePointCloud : public SceneObject {
 public:
  void Draw();
  ObjectType GetType() { return PointCloud; }
  void AddPoint(libmv::Vec3 &);
  
  ~ScenePointCloud() {};
 private:
  // Feel free to change this to a list if removal of points is needed (Daniel).
  std::vector<libmv::Vec3> points_;
};

class SceneImage : public SceneObject {
 public:
  void Draw();
  ObjectType GetType() { return Image; }
  SceneImage(GLTexture &texture) : texture_(texture) {}
  
  ~SceneImage() {};
 private:
  GLTexture texture_;
};

// A widget displaying a 3D scene, including:
//  - Cameras.
//  - Point clouds.
//  - Images.
class Viewer3D : public QGLWidget {
  Q_OBJECT
  
 public:
  Viewer3D(QGLWidget *share, GLTexture *textures, QWidget *parent);
  ~Viewer3D() {};

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

 public slots:
  void SetDocument(TvrDocument *);
  void GLUpdate() { updateGL(); }
  void TextureChange();
  void InitImages();

 protected:
  // Drawing.
  void initializeGL();
  void paintGL();
  void resizeGL(int, int);
  
  void SetUpGlCamera();

  // Mouse.
  void mousePressEvent(QMouseEvent *) {};
  void mouseMoveEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent *);

 private:
  TvrDocument *document_;
  GLTexture *textures_;
  
  libmv::SceneGraph<SceneObject> scene_graph_;
  
  QPoint lastPos_;
};

#endif // UI_TVR_3D_VIEWER_H_
