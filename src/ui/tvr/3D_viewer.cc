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
static void Draw(libmv::SGNode<SceneObject> *ptr,  void *) {
  glLoadMatrixf(ptr->GetMatrix().data());
  ptr->GetObject()->draw();
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
  std::vector<ScenePoint>::iterator it;
  glBegin(GL_POINTS);
  for (it=points_.begin(); it!=points_.end(); ++it) {
    glVertex3f(it->x_, it->y_, it->z_);
  }
  glEnd();
}

void ScenePointCloud::AddPoint(ScenePoint &s) {
  points_.push_back(s);
}

void SceneImage::Draw() {
  
}

Viewer3D::Viewer3D(QWidget *parent) : QGLWidget(parent), document_(NULL) {}

void Viewer3D::SetDocument(TvrDocument *doc) {
  document_ = doc;
}
