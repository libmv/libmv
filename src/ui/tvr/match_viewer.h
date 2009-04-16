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

#ifndef UI_TVR_MATCH_VIEWER_H_
#define UI_TVR_MATCH_VIEWER_H_

#include <vector>

#include <QGLWidget>

#include "ui/tvr/tvr_document.h"
#include "ui/tvr/on_screen_image.h"

// A widget displaying two images on a plane, with surf features and matches.
//  - Dragging moves the plane.
//  - Scrolling zooms.
//  - Shift-dragging moves the selected image.
class MatchViewer : public QGLWidget {
  Q_OBJECT

 public:
  MatchViewer(QGLWidget *share, OnScreenImage *);
  ~MatchViewer();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

 public slots:
  void SetDocument(TvrDocument *doc);
  void SetTransformation(float tx_, float ty_, float zoom_);

 protected:
  // Drawing.
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void SetUpGlCamera();
  void DrawImage(int i);
  void DrawFeatures(int image_index);
  void DrawCandidateMatches();
  bool TexturesInited();
  
  // Mouse.
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  int ImageUnderPointer(QMouseEvent *event);

  // Coordinate systems.
  void PlaneFromScreen(float xw, float yw, float *xi, float *yi);
  void ScreenFromPlane(float xi, float yi, float *xw, float *yw);

 private:
  TvrDocument *document_;
  OnScreenImage *screen_images_;

  enum MouseDragBehavior {
    NONE, MOVE_VIEW, MOVE_IMAGE
  } mouse_drag_behavior_;
  
  int dragging_image_;
    
  float tx, ty;  // Top left corner of the window in plane coordinates.
  float zoom;    // Window pixels per plane pixel.

  QPoint lastPos;
};

#endif // UI_TVR_MATCH_VIEWER_H_
