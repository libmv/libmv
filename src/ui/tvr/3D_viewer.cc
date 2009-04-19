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

#include <vector>

#include <QtGui>
#include <QtOpenGL>

#include "ui/tvr/3D_viewer.h"

#define ROTATION_SPEED 1.0f
#define TRANSLATION_SPEED 1.0f

static bool Draw(libmv::SGNode<SceneObject> *ptr,  void *) {
  glLoadMatrixd(ptr->GetMatrix().data());
  ptr->GetObject()->Draw();
  return true;
}

void SceneCamera::Draw() {
  glBegin(GL_LINES);
  // Feel free to change the way camera's are represented (Daniel).
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(1.0f,1.0f,1.0f);
  
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(1.0f,-1.0f,1.0f);
  
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(-1.0f,1.0f,1.0f);
  
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(-1.0f,-1.0f,1.0f);
  
  glVertex3f(-1.0f,-1.0f,1.0f);
  glVertex3f(-1.0f,1.0f,1.0f);
  
  glVertex3f(-1.0f,1.0f,1.0f);
  glVertex3f(1.0f,1.0f,1.0f);
  
  glVertex3f(1.0f,1.0f,1.0f);
  glVertex3f(1.0f,-1.0f,1.0f);
  
  glVertex3f(1.0f,-1.0f,1.0f);
  glVertex3f(-1.0f,-1.0f,1.0f);
  glEnd();
}

void ScenePointCloud::Draw() {
  std::vector<libmv::Vec3>::iterator it;
  glBegin(GL_POINTS);
  for (it=points_.begin(); it!=points_.end(); ++it) {
    glVertex3f(it->x(), it->y(), it->z());
  }
  glEnd();
}

void ScenePointCloud::AddPoint(libmv::Vec3 &s) {
  points_.push_back(s);
}

void SceneImage::Draw() {
  assert(glIsTexture(texture_.textureID));

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_.textureID);
  
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0); glVertex2d(-0.5f, -0.5f);
  glTexCoord2d(1, 0); glVertex2d(0.5f, -0.5f);
  glTexCoord2d(1, 1); glVertex2d(0.5f, 0.5f);
  glTexCoord2d(0, 1); glVertex2d(-0.5f, 0.5f);
  glEnd();
  
  glDisable(GL_TEXTURE_2D);
}

Viewer3D::Viewer3D(QGLWidget *share, GLTexture *textures, QWidget *parent) :
    QGLWidget(0, share), document_(NULL), textures_(textures) {
  using namespace Eigen;
  
  connect(parent, SIGNAL(GLUpdateNeeded()), this, SLOT(GLUpdate()));
  connect(parent, SIGNAL(TextureChanged()), this, SLOT(TextureChange()));
  
  Transform3d transform;
  transform.matrix() = scene_graph_.GetObjectMatrix();
  transform = Translation3d(0,0,-1) * transform;
  scene_graph_.GetObjectMatrix() = transform.matrix();
  scene_graph_.UpdateMatrix();
  
  setWindowTitle("3D View");
}

void Viewer3D::SetDocument(TvrDocument *doc) {
  document_ = doc;
  
  assert(document_);
  assert(textures_);
  
  if (!document_->images[0].isNull() && !document_->images[1].isNull())
    InitImages();
  
  ScenePointCloud *p = new ScenePointCloud;
  
  std::vector<libmv::Vec3>::iterator it;
  for (it=document_->X.begin(); it!=document_->X.end(); ++it) {
    p->AddPoint(*it);
  }
  
  scene_graph_.AddChild(
      libmv::MakeSGNode<SceneObject>(p, "PointCloud"));
}

QSize Viewer3D::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize Viewer3D::sizeHint() const {
  return QSize(800, 400);
}

void Viewer3D::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glShadeModel(GL_FLAT);
}

void Viewer3D::paintGL() {
  SetUpGlCamera();
  scene_graph_.ForeachChildRecursive(&Draw, NULL);
}

void Viewer3D::SetUpGlCamera() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1.5f, 0, 10);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
}

void Viewer3D::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

static bool IsImage(libmv::SGNode<SceneObject> *ptr, void *) {
  return ptr->GetObject()->GetType() == SceneObject::Image;
}

void Viewer3D::TextureChange() {
  scene_graph_.DeleteIfRecursive(&IsImage, NULL);
  
  InitImages();
}

void Viewer3D::InitImages() {
  using namespace libmv;
  using namespace Eigen;
  
  SGNode<SceneObject> *ptr =
      MakeSGNode<SceneObject>(new SceneImage(textures_[0]), "Image01");
  scene_graph_.AddChild(ptr);
  
  Transform3d transform;
  transform.matrix() = ptr->GetObjectMatrix();
  transform = Translation3d(1,0,0) * transform;
  ptr->GetObjectMatrix() = transform.matrix();
  ptr->UpdateMatrix();
  
  ptr = MakeSGNode<SceneObject>(new SceneImage(textures_[1]), "Image02");
  scene_graph_.AddChild(ptr);
}

void Viewer3D::wheelEvent(QWheelEvent *) {
  // Doesn't seem to ever get called ??? (Daniel)
}

void Viewer3D::mouseMoveEvent(QMouseEvent *event) {
  using namespace Eigen;
  QPoint delta = event->pos() - lastPos_;
  lastPos_ = event->pos();  
  
  if(!(delta.x()||delta.y()))
    return;
  
  if (event->buttons() & (Qt::LeftButton | Qt::RightButton)) {
    Transform3d transform;
    transform.matrix() = scene_graph_.GetObjectMatrix();
    if (event->buttons() & Qt::LeftButton) {
      transform = AngleAxisd(-delta.y() * ROTATION_SPEED, Vector3f::UnitX())
                * AngleAxisd(-delta.x() * ROTATION_SPEED, Vector3f::UnitY())
                * transform;
    }
    if (event->buttons() & Qt::RightButton) {
      transform = Translation3d
                      (delta.y() * TRANSLATION_SPEED * Vector3f::UnitZ())
                * AngleAxisd( - delta.x() * ROTATION_SPEED, Vector3f::UnitZ())
                * transform;
    }
    scene_graph_.GetObjectMatrix() = transform.matrix();
    scene_graph_.UpdateMatrix();
    updateGL();
  }
}
