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

#include "ui/tvr/3D_viewer.h"

#ifdef NOT_IMPLEMENTED_YET 
static bool Draw(libmv::SGNode<SceneObject> *ptr,  void *) {
  glLoadMatrixf(ptr->GetMatrix().data());
  ptr->GetObject()->draw();
  return true;
}
// To draw the scene then becomes, scene_graph.ForeachChildRecursive(&Draw, NULL);
#endif

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
  assert(glIsTexture(texture_));

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_);
  
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0); glVertex2d(-1, -1);
  glTexCoord2d(1, 0); glVertex2d(1, -1);
  glTexCoord2d(1, 1); glVertex2d(1, 1);
  glTexCoord2d(0, 1); glVertex2d(-1, 1);
  glEnd();
  
  glDisable(GL_TEXTURE_2D);
}

Viewer3D::Viewer3D(QGLWidget *share, OnScreenImage *images) : QGLWidget(0, share), document_(NULL), screen_images_(images) {}

void Viewer3D::SetDocument(TvrDocument *doc) {
  document_ = doc;
}

QSize Viewer3D::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize Viewer3D::sizeHint() const {
  return QSize(800, 400);
}
