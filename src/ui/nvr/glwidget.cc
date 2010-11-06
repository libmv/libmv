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

#include <QtOpenGL> 
#include "glwidget.h"

using namespace libmv;

GLWidget::GLWidget(QWidget *parent) 
 :QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
  viewer_position_ << 0, 0,  -10;
  viewer_orientation_ << 0, 0, 0;
}
GLWidget::~GLWidget(){
  foreach(GLuint s, structures_list_) {
    glDeleteLists(s, 1);
  }
}
void GLWidget::initializeGL()
{
  QGLWidget::initializeGL();
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_MULTISAMPLE);
  static GLfloat light_position[4] = { 0, 0, 50.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void GLWidget::resizeGL(int w, int h)
{
  QGLWidget::resizeGL(w, h);
}

void GLWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-5, 5, -5, 5, 1, 1000);
  glTranslatef(viewer_position_[0], viewer_position_[1], viewer_position_[2]);
  glRotatef(viewer_orientation_[0], 1.0, 0.0, 0.0);
  glRotatef(viewer_orientation_[1], 0.0, 1.0, 0.0);
  glRotatef(viewer_orientation_[2], 0.0, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, width(), height());
  glDisable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
  
  // NOTE just to have something...
  // remove this and adds x, y and z axis
  glBegin(GL_TRIANGLES);           // Drawing Using Triangles
    glVertex3f( 0.0f, 1.0f, 0.0f);        // Top
    glVertex3f(-1.0f,-1.0f, 0.0f);        // Bottom Left
    glVertex3f( 1.0f,-1.0f, 0.0f);        // Bottom Right
  glEnd();
    
  foreach(GLuint s, structures_list_) {
    glCallList(s);
  }
}
void GLWidget::mousePressEvent(QMouseEvent *event) {
  last_mouse_position_ = event->pos();  
}

static void NormalizeAngle(float &angle) {
  while(angle < 0)
    angle += 360;
  while(angle > 360)
    angle -= 360;
}
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - last_mouse_position_.x();
  int dy = event->y() - last_mouse_position_.y();
  if (event->buttons() & Qt::LeftButton) {
    viewer_orientation_[0] += 0.5 * dy;
    viewer_orientation_[1] += 0.5 * dx;
    NormalizeAngle(viewer_orientation_[0]);
    NormalizeAngle(viewer_orientation_[1]);
  } else if (event->buttons() & Qt::RightButton) {
    viewer_orientation_[0] += 0.5 * dy;
    viewer_orientation_[2] += 0.5 * dx;
    NormalizeAngle(viewer_orientation_[0]);
    NormalizeAngle(viewer_orientation_[2]);
  }
  last_mouse_position_ = event->pos();
  updateGL();
}
void GLWidget::wheelEvent(QWheelEvent *event) {
  float scale = 1.0f;
  if (event->delta() < 0)
    scale = 1.2;
  else
    scale = 0.8;
  if (event->orientation() == Qt::Horizontal) {
    viewer_position_[0] *= scale;
  } else {
    viewer_position_[2] *= scale;
  } 
  updateGL();
}

inline void GLWidget::DrawPointStructure(libmv::Vec3 &p) {
//  glVertex3f(p[0], p[1], p[2]);
  glVertex3f(p[0]-0.1, p[1]-0.1, p[2]+0.1);
  glVertex3f(p[0]+0.1, p[1]-0.1, p[2]+0.1);
  glVertex3f(p[0]+0.1, p[1]+0.1, p[2]+0.1);
  glVertex3f(p[0]-0.1, p[1]+0.1, p[2]+0.1);
}
void GLWidget::AddNewStructure(vector<Vec3> &struct_coords) {
  GLuint s_new = glGenLists(1);
  structures_list_ << s_new;
  glNewList(s_new, GL_COMPILE);
  //glBegin(GL_POINT);
  glBegin(GL_QUADS);
  for (size_t s = 0; s < struct_coords.size(); ++s) {
    DrawPointStructure(struct_coords[s]);
  }
  glEnd();
  glEndList();
  updateGL();
}
