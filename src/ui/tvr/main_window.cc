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

#include <QMenuBar>
#include <QFileDialog>

#include "libmv/image/array_nd.h"
#include "libmv/image/surf.h"
#include "libmv/image/surf_descriptor.h"
#include "ui/tvr/main_window.h"


TvrMainWindow::TvrMainWindow(QWidget *parent)
  : QMainWindow(parent) {

  viewer_ = new MatchViewer();
  setCentralWidget(viewer_);

  CreateActions();
  CreateMenus();
}

TvrMainWindow::~TvrMainWindow() {
}

void TvrMainWindow::CreateActions() {
  open_images_action_ = new QAction(tr("&Open Images..."), this);
  open_images_action_->setShortcut(tr("Ctrl+O"));
  open_images_action_->setStatusTip(tr("Open an image pair"));
  connect(open_images_action_, SIGNAL(triggered()),
          this, SLOT(OpenImages()));
  
  compute_features_action_ = new QAction(tr("&Compute Features..."), this);
  compute_features_action_->setStatusTip(tr("Compute Surf Features"));
  connect(compute_features_action_, SIGNAL(triggered()),
          this, SLOT(ComputeFeatures()));
  
  compute_candidate_matches_ = new QAction(tr("&Compute Candidate Matches..."),
                                           this);
  compute_candidate_matches_->setStatusTip(tr("Compute Candidate Matches"));
  connect(compute_candidate_matches_, SIGNAL(triggered()),
          this, SLOT(ComputeCandidateMatches()));
}

void TvrMainWindow::CreateMenus() {
  file_menu_ = menuBar()->addMenu(tr("&File"));
  file_menu_->addAction(open_images_action_);
  matching_menu_ = menuBar()->addMenu(tr("&Matching"));
  matching_menu_->addAction(compute_features_action_);
  matching_menu_->addAction(compute_candidate_matches_);
}

void TvrMainWindow::OpenImages() {
  QStringList filenames = QFileDialog::getOpenFileNames(this,
      "Select Two Images", "", "Image Files (*.png *.jpg *.bmp *.pgm *.xpm)");

  for (int i = 0; i < 2; ++i) {
    document_.images[i].load(filenames[i]);
  }

  viewer_->SetDocument(&document_);
}

void TvrMainWindow::ComputeFeatures() {
  for (int i = 0; i < 2; ++i) {
    ComputeFeatures(i);
  }
}
  
void TvrMainWindow::ComputeFeatures(int image_index) {
  QImage &qimage = document_.images[image_index];
  int width = qimage.width(), height = qimage.height();
  SurfFeatureSet &fs = document_.feature_sets[image_index];
      
  // Convert to gray-scale.
  libmv::Array3Df image(height, width, 1);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // TODO(pau): there are better ways to compute intensity.
      //            Implement one and put it on libmv/image.
      image(y, x) = (float(qimage.bits()[4*(y * width + x) + 2])
                   + float(qimage.bits()[4*(y * width + x) + 1])
                   + float(qimage.bits()[4*(y * width + x) + 0])) / 3;
    }
  }

  // Detect features.
  std::vector<libmv::Vec3f> detections;
  libmv::MultiscaleDetectFeatures(image, 4, 4, &detections);
  fs.features.resize(detections.size());


  // Compute descriptors and fill surf_feature_set structure.
  libmv::Matf integral_image;
  libmv::IntegralImage(image, &integral_image);
  for (int i = 0; i < detections.size(); ++i) {
    SurfFeature &f = fs.features[i];
    f.x = detections[i](2);
    f.y = detections[i](1);
    f.scale = detections[i](0);
    f.orientation = 0;
    libmv::Matrix<float, 64, 1> descriptor;
    libmv::USURFDescriptor<4, 5>(integral_image, f.x, f.y, f.scale,
                                 &descriptor);
    for (int k = 0; k < 64; ++k) {
      f.descriptor[k] = descriptor(k);
    }
  }

  // Build the kd-tree.
  fs.tree.Build(&fs.features[0], fs.features.size(), 64, 10);

  viewer_->updateGL();
}

void TvrMainWindow::ComputeCandidateMatches() {
  FindCandidateMatches(document_.feature_sets[0],
                       document_.feature_sets[1],
                       &document_.candidate_matches);
  
  viewer_->updateGL();
}
