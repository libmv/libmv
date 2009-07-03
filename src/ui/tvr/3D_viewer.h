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

#if _WIN32
#define NOMINMAX // Eigen needs this
#endif //_WIN32

#include <QGLWidget>
#include <QImage>

#include "libmv/base/vector.h"
#include "libmv/scene_graph/scene_graph_simple.h"
#include "ui/tvr/tvr_document.h"
#include "ui/tvr/gl_texture.h"

class SceneObject {
 public:
  virtual ~SceneObject() {};
  
  enum ObjectType {
    Camera, PointCloud, Image
  };
  
  // The drawing function should assume that
  // opengl's model matrix is already ready for drawing.
  // Changing the model matrix will affect children.
  virtual void Draw() {};
  virtual ObjectType GetType() = 0;
};

class SceneCamera : public SceneObject {
 public:
  void Draw();
  ObjectType GetType() { return Camera; }
  void SetParameters(libmv::Mat3 K, libmv::Mat3 R, libmv::Vec3 t);
  void SetTexture(GLTexture texture) { texture_ = texture; }
  
  ~SceneCamera() {};
 private:
  libmv::Mat3 K_;
  libmv::Mat3 R_;
  libmv::Vec3 t_;
  GLTexture texture_;
};

class ScenePointCloud : public SceneObject {
 public:
  void Draw();
  ObjectType GetType() { return PointCloud; }
  void AddPoint(libmv::Vec3 &point, libmv::Vec3f &color);
  
  ~ScenePointCloud() {};
 private:
  std::vector<libmv::Vec3> points_;
  std::vector<libmv::Vec3f> colors_;
};


class ViewerCamera {
 public:
  ViewerCamera();
  void SetScreenSize(int width, int height);
  void SetUpGlCamera();
  void MouseTranslate(float x1, float y1, float x2, float y2);
  void MouseRevolve(float x1, float y1, float x2, float y2);
  void MouseZoom(float dw);

 private:
  // Intrinsic parameters.
  float field_of_view_;
  float near_;
  float far_;
  float screen_width_;
  float screen_height_;
  
  // Extrinsic parameters.
  // The parameters define the transformation between the world and the camera
  // frames as
  //     cam_coords = Rotation(orientation_) * (world_coords - revolve_point_)
  //                + revolve_point_in_cam_coords_.
  libmv::Vec3f revolve_point_;
  libmv::Vec3f revolve_point_in_cam_coords_; // Implicitly defines the position
                                             // of the camera.
  Eigen::Quaternionf orientation_; // Orientation of the world axis w.r.t.
                                   // the camera axis.

  // Interaction parameters.
  float translation_speed_;
  float zoom_speed_;
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

 protected:
  // Drawing.
  void initializeGL();
  void paintGL();
  void resizeGL(int, int);

  // Mouse.
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent *);

 private:
  TvrDocument *document_;
  GLTexture *textures_;
  
  ScenePointCloud scene_point_cloud_;
  SceneCamera scene_cameras_[2];
  libmv::scene::Node<SceneObject> scene_graph_;
  ViewerCamera viewer_camera_;
  
  QPoint lastPos_;
};

#endif // UI_TVR_3D_VIEWER_H_
