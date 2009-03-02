// Copyright (c) 2007, 2008 libmv authors.
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


#include <QtGui>
#include <QtOpenGL>
#include <cmath>

#include "ui/tvr/match_viewer.h"


MatchViewer::MatchViewer(QWidget *parent)
: QGLWidget(parent), document_(NULL) {
  tx = 0;
  ty = 0;
  zoom = 1;
}

MatchViewer::~MatchViewer() {
  makeCurrent();
}

void MatchViewer::SetDocument(TvrDocument *doc) {
  document_ = doc;
  
  for (int i = 0; i < 2; ++i) {
    UpdateScreenImage(i);
  }
}

void MatchViewer::UpdateScreenImage(int index) {
  OnScreenImage &oi = screen_images_[index];
  QImage &im = document_->images[index];

  glGenTextures(1, &oi.textureID);

  oi.width = im.width();
  oi.height = im.height();

  unsigned char *data = new unsigned char[oi.width * oi.height * 4];

  for (int i = 0; i < oi.height; ++i) {
    for (int j = 0; j < oi.width; ++j) {
      data[4*(i * oi.width + j) + 0] = im.bits()[4*(i * oi.width + j) + 2];
      data[4*(i * oi.width + j) + 1] = im.bits()[4*(i * oi.width + j) + 1];
      data[4*(i * oi.width + j) + 2] = im.bits()[4*(i * oi.width + j) + 0];
    }
  }

  // Select our current texture.
  glBindTexture(GL_TEXTURE_2D, oi.textureID);
  // Select modulate to mix texture with color for shading.
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // When texture area is small, bilinear filter the closest mipmap.
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  // When texture area is large, enlarge the pixels but don't upsample.
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // Wrap the texture at the edges (repeat).
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // build our texture mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, oi.width, oi.height,
      GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete [] data;

  if (index == 0) {
    oi.posx = 0;
    oi.posy = 0;
  } else {
    OnScreenImage &prev = screen_images_[index - 1];
    oi.posx = prev.posx + prev.width + 10;
    oi.posy = prev.posy;
  }

  updateGL();
}

QSize MatchViewer::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize MatchViewer::sizeHint() const {
  return QSize(800, 400);
}

void MatchViewer::SetTransformation(float tx_, float ty_, float zoom_) {
  tx = tx_;
  ty = ty_;
  zoom = zoom_;
  updateGL();
}

void MatchViewer::PlaneFromScreen(float xw, float yw, float *xi, float *yi) {
  *xi = zoom * xw + tx;
  *yi = zoom * yw + ty;
}

void MatchViewer::ScreenFromPlane(float xi, float yi, float *xw, float *yw) {
  *xw = (xi - tx) / zoom;
  *yw = (yi - ty) / zoom;
}

void MatchViewer::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glShadeModel(GL_FLAT);
}


// Sets up GL_PROJECTION so that we can start drawing in plane coordinates.
void MatchViewer::SetUpGlCamera() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float x0, y0, x1, y1;
  PlaneFromScreen(0, 0, &x0, &y0);
  PlaneFromScreen(width(), height(), &x1, &y1);
  glOrtho(x0, x1, y1, y0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
}

void MatchViewer::DrawImage(int i) {
  OnScreenImage &si = screen_images_[i];

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(si.posx, si.posy, 0);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, si.textureID);
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0); glVertex2d(0, 0);
  glTexCoord2d(1, 0); glVertex2d(si.width, 0);
  glTexCoord2d(1, 1); glVertex2d(si.width, si.height);
  glTexCoord2d(0, 1); glVertex2d(0, si.height);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  DrawFeatures(i);

  glPopMatrix();
}

void MatchViewer::DrawFeatures(int image_index) {
  std::vector<libmv::SurfFeature> &features =
      document_->feature_sets[image_index].features;
  
  for (int i = 0; i < features.size(); ++i) {
    glPushMatrix();
    glTranslatef(features[i].x(), features[i].y(), 0);
    glScalef(features[i].scale, features[i].scale, features[i].scale);
    // TODO(pau) when surf orientation will be detected, ensure that this is
    //           turning in the right sense and the right units (deg vs rad).
    glRotatef(features[i].orientation, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2f(-1, -1); glVertex2f( 1, -1); // Square Box.
    glVertex2f( 1, -1); glVertex2f( 1,  1);
    glVertex2f( 1,  1); glVertex2f(-1,  1);
    glVertex2f(-1,  1); glVertex2f(-1, -1);
    glVertex2f(0,  0); glVertex2f(0, -1);   // Vertical line to see orientation.
    glEnd();
    glPopMatrix();
  }
}

void MatchViewer::DrawCandidateMatches() {
  float xoff[2] = { screen_images_[0].posx, screen_images_[1].posx};
  float yoff[2] = { screen_images_[0].posy, screen_images_[1].posy};
  
  glBegin(GL_LINES);  // TODO(keir): Note that this will break with > 2 images.
  for (libmv::Correspondences::TrackIterator t =
          document_->correspondences.ScanAllTracks();
       !t.Done(); t.Next()) {
    for (libmv::PointCorrespondences::Iterator it =
            libmv::PointCorrespondences(&document_->correspondences)
              .ScanFeaturesForTrack(t.track());
         !it.Done(); it.Next()) {
      glVertex2f(xoff[it.image()] +  it.feature()->x(),
                 yoff[it.image()] +  it.feature()->y());
    }
  }
  glEnd();
}


void MatchViewer::paintGL() {
  if (document_) {
    SetUpGlCamera();
    for (int i = 0; i < 2; ++i) {
      DrawImage(i);
    }
    DrawCandidateMatches();
  }
}

void MatchViewer::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

int MatchViewer::ImageUnderPointer(QMouseEvent *event) {
  float x, y;
  PlaneFromScreen(event->x(), event->y(), &x, &y);

  for (int i = 0; i < 2; ++i) {
    if (screen_images_[i].Contains(x, y)) {
      return i;
    }
  }
  return -1; 
}

void MatchViewer::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
  
  // Set mouse drag behavior.
  mouse_drag_behavior_ = NONE;
  if (event->modifiers() & Qt::ShiftModifier) {
    int i = ImageUnderPointer(event);
    if (i >= 0) {
      dragging_image_ = i;
      mouse_drag_behavior_ = MOVE_IMAGE;
    }
  } else {
    mouse_drag_behavior_ = MOVE_VIEW;
  }
}

void MatchViewer::mouseMoveEvent(QMouseEvent *event) {
  float x0, y0, x1, y1;
  PlaneFromScreen(lastPos.x(), lastPos.y(), &x0, &y0);
  PlaneFromScreen(event->x(), event->y(), &x1, &y1);

  if (event->buttons() & Qt::LeftButton) {
    if (mouse_drag_behavior_ == MOVE_IMAGE) {
      // Move the first image under the pointer.
      screen_images_[dragging_image_].posx += x1 - x0;
      screen_images_[dragging_image_].posy += y1 - y0;
      updateGL();
    } else if (mouse_drag_behavior_ == MOVE_VIEW) {
      // Move view.
      SetTransformation(tx + x0 - x1, ty + y0 - y1, zoom); 
    }
  }
  lastPos = event->pos();
}

void MatchViewer::wheelEvent(QWheelEvent *event) {
  // Zoom so that the image point under the cursor does not move.
  const float newzoom = zoom * pow(1.001, event->delta());
  const float newtx = (zoom - newzoom) * event->x() + tx;
  const float newty = (zoom - newzoom) * event->y() + ty;

  SetTransformation(newtx, newty, newzoom);
}
