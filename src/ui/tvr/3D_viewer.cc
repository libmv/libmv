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

#define ROTATION_SPEED 0.003f
#define TRANSLATION_SPEED 0.01f

using namespace libmv::scene;

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
  glDisable(GL_LIGHTING);
  glColor3f(0,1,0);
  glBegin(GL_POINTS);
  for (it = points_.begin(); it != points_.end(); ++it) {
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

ViewerCamera::ViewerCamera() {  
  field_of_view_ = 60;
  near_ = 0.1;
  far_ = 100;
  screen_width_ = 1;
  screen_height_ = 1;
  
  position_ << 0, 0, -1;
  orientation_ = Eigen::Quaternionf(1, 0, 0, 0);
  
  revolve_point_ << 0, 0, 0;
  revolve_speed_ = 1;
  translation_speed_ = 1;
  zoom_speed_ = 0.002;
}
  
void ViewerCamera::SetScreenSize(int width, int height) {
  screen_width_ = width;
  screen_height_ = height;
}

void ViewerCamera::SetUpGlCamera() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(field_of_view_,
                 float(screen_width_) / float(screen_height_),
                 near_, far_);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(position_(0), position_(1), position_(2));
  
  libmv::Mat4f R = libmv::Mat4f::Identity();
  R.block<3,3>(0,0) = orientation_.toRotationMatrix();
  glMultMatrixf(&R(0,0));
}

void ViewerCamera::MouseTranslate(float dx, float dy) {
  position_(0) += translation_speed_ * dx / screen_width_;
  position_(1) += translation_speed_ * dy / screen_width_;
};

void ViewerCamera::MouseRevolve(float dx, float dy) {
  Eigen::Quaternionf dr(1, dy / screen_width_, dx / screen_width_, 0);
  dr.normalize();
  orientation_ *= dr;
  orientation_.normalize();
}

void ViewerCamera::MouseZoom(float dw) {
  position_(2) += zoom_speed_ * dw;
}

Viewer3D::Viewer3D(QGLWidget *share, GLTexture *textures, QWidget *parent) :
    QGLWidget(0, share), document_(NULL), textures_(textures) {
  using namespace Eigen;
  
  connect(parent, SIGNAL(GLUpdateNeeded()), this, SLOT(GLUpdate()));
  connect(parent, SIGNAL(TextureChanged()), this, SLOT(TextureChange()));
  
  scene_graph_ = Node<SceneObject>("root node", NULL);
  
  Transform3d transform;
  transform.matrix() = scene_graph_.GetTransform();
  transform = Translation3d(0,0,-1) * transform;
  scene_graph_.SetTransform(transform.matrix());
  
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
  for (it = document_->X.begin(); it != document_->X.end(); ++it) {
    p->AddPoint(*it);
  }
  
  scene_graph_.AddChild(new Node<SceneObject>("PointCloud", p));
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

static void DrawNode(Node<SceneObject> *node) {
  glPushMatrix();
  if(node->GetObject()) {
    glMultMatrixd(node->GetTransform().data());
    node->GetObject()->Draw();
  }
  Node<SceneObject>::iterator it;
  for (it=node->begin(); it!=node->end(); ++it) {
    DrawNode(&*it);
  }
  glPopMatrix();
}

void Viewer3D::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  camera_.SetUpGlCamera();
   
  glBegin(GL_LINES);
  glColor4f(1,0,0,1); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
  glColor4f(0,1,0,1); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
  glColor4f(0,0,1,1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
  glEnd();
  
  glColor4f(1,1,1,1);
  DrawNode(&scene_graph_);
}

void Viewer3D::resizeGL(int width, int height) {
  camera_.SetScreenSize(width, height);
  glViewport(0, 0, width, height);
}

void Viewer3D::TextureChange() {
  Node<SceneObject>::iterator it;
  for (it = scene_graph_.begin(); it != scene_graph_.end();) {
    if (it->GetObject()->GetType() == SceneObject::Image)
      it = scene_graph_.erase(it);
    else
      ++it;
  }
  
  InitImages();
}

void Viewer3D::InitImages() {
  using namespace Eigen;
  
  Node<SceneObject> *ptr =
      new Node<SceneObject>("Image01", new SceneImage(textures_[0]));
  
  scene_graph_.AddChild(ptr);

  Transform3d transform;
  transform.matrix() = ptr->GetTransform();
  transform = Translation3d(1,0,0) * transform;
  ptr->SetTransform(transform.matrix());
  
  ptr = new Node<SceneObject>("Image02", new SceneImage(textures_[1]));
  scene_graph_.AddChild(ptr);
}

void Viewer3D::mousePressEvent(QMouseEvent *event) {
  lastPos_ = event->pos();
}

void Viewer3D::mouseMoveEvent(QMouseEvent *event) {
  QPoint delta = event->pos() - lastPos_;
  lastPos_ = event->pos();  
  
  if(delta.x() == 0 && delta.y() == 0) {
    return;
  }
  
  if (event->buttons() & Qt::LeftButton) {
    camera_.MouseRevolve(delta.x(), delta.y());
  }
  
  if (event->buttons() & Qt::RightButton) {
    camera_.MouseTranslate(delta.x(), delta.y());
  }
  updateGL();
}

void Viewer3D::wheelEvent(QWheelEvent *event) {
  camera_.MouseZoom(event->delta());
  updateGL();
}
