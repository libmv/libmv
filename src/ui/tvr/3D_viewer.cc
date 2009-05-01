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
  
  revolve_point_ << 0, 0, 0;
  revolve_point_in_cam_coords_ << 0, 0, -1;
  orientation_ = Eigen::Quaternionf(1, 0, 0, 0);
  
  translation_speed_ = 5;
  zoom_speed_ = 0.002;
}
  
void ViewerCamera::SetScreenSize(int width, int height) {
  screen_width_ = width;
  screen_height_ = height;
}

void ViewerCamera::SetUpGlCamera() {
  // Set intrinsic parameters.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(field_of_view_,
                 float(screen_width_) / float(screen_height_),
                 near_, far_);
  
  // Set extrinsic parametres.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // (1) Translate axis to revolve_point.
  glTranslatef(revolve_point_in_cam_coords_(0),
               revolve_point_in_cam_coords_(1),
               revolve_point_in_cam_coords_(2)); 
  // (2) Rotate.
  Eigen::AngleAxisf aa = orientation_;
  glRotatef(aa.angle() * 180. / M_PI, aa.axis()(0), aa.axis()(1), aa.axis()(2));
  // (3) Translate axis to world origin.
  glTranslatef(-revolve_point_(0), -revolve_point_(1), -revolve_point_(2));
}

void ViewerCamera::MouseTranslate(float x1, float y1, float x2, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  revolve_point_in_cam_coords_(0) += translation_speed_ * dx / screen_width_;
  revolve_point_in_cam_coords_(1) -= translation_speed_ * dy / screen_width_;
};

static libmv::Vec3f LiftToTrackball(float x, float y,
                                    float width, float height) {
  float sphere_radius = std::min(width, height) / 2;

  // Normalize coordinates, and reverse y axis.
  x = (x - width / 2) / sphere_radius;
  y = - (y - height / 2) / sphere_radius;

  float r2 = x * x + y * y;
  float z;
  if (r2 < 0.5) {     
    z = sqrt(1 - r2);        // Lift to the sphere.
  } else {                    
    z = 1 / (2 * sqrt(r2));  // Lift to the hyperboloid.
  }
  return libmv::Vec3f(x, y, z);
}

void ViewerCamera::MouseRevolve(float x1, float y1, float x2, float y2) {
  if (x1 == x2 && y1 == y2) {
    return;
  }
  // Lift points to the trackball.
  libmv::Vec3f p1 = LiftToTrackball(x1, y1, screen_width_, screen_height_);
  libmv::Vec3f p2 = LiftToTrackball(x2, y2, screen_width_, screen_height_);

  // Compute rotation between the lifted vectors.
  Eigen::Quaternionf dq;
  dq.setFromTwoVectors(p1, p2);
  dq.normalize();
  
  // Apply the rotation.
  orientation_ = dq * orientation_;
  orientation_.normalize();
}

void ViewerCamera::MouseZoom(float dw) {
  revolve_point_in_cam_coords_(2) += - zoom_speed_ * dw;
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
  float x1 = lastPos_.x();
  float y1 = lastPos_.y();
  float x2 = event->pos().x();
  float y2 = event->pos().y();
  
  if(x1 == x2 && y1 == y2) {
    return;
  }
  
  if (event->buttons() & Qt::LeftButton) {
    camera_.MouseRevolve(x1, y1, x2, y2);
  }
  
  if (event->buttons() & Qt::RightButton) {
    camera_.MouseTranslate(x1, y1, x2, y2);
  } 
  
  lastPos_ = event->pos();
  updateGL();
}

void Viewer3D::wheelEvent(QWheelEvent *event) {
  camera_.MouseZoom(event->delta());
  updateGL();
}
