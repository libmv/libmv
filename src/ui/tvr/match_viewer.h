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

#ifndef TOOLS_TVR_MATCH_VIEWER_H_
#define TOOLS_TVR_MATCH_VIEWER_H_


#include <QGLWidget>
#include <vector>


// A widget displaying multiple images on a plane.
//  - Dragging moves the plane.
//  - Scrolling zooms.
// In the future:
//  - Shift-dragging moves the selected image.
//  - Shift-scrolling scales the selected image.
class MatchViewer : public QGLWidget {
  Q_OBJECT

  struct OnScreenImage {
    GLuint textureID;
    int width, height;
    float posx, posy;
    float scale;
  };


 public:
  MatchViewer(QWidget *parent = 0);
  ~MatchViewer();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

 public slots:
  void SetImages(const QImage *images, int n);
  void AddImage(const QImage &im);
  void SetTransformation(float tx_, float ty_, float zoom_);

 protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

  // Drawing methods.
  void SetUpGlCamera();
  void DrawImages();

  // Coordinate systems.
  void PlaneFromScreen(float xw, float yw, float *xi, float *yi);
  void ScreenFromPlane(float xi, float yi, float *xw, float *yw);


 private:
  std::vector<OnScreenImage> screen_images_;

  float tx;    /// Top left corner of the window in plane coordinates.
  float ty;
  float zoom;  /// Window pixels per plane pixel.

  QPoint lastPos;
};

#endif // TOOLS_TVR_MATCH_VIEWER_H_
