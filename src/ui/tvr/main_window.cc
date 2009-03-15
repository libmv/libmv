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
#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/logging/logging.h"
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
  
  compute_features_action_ = new QAction(tr("&Compute Features"), this);
  compute_features_action_->setStatusTip(tr("Compute Surf Features"));
  connect(compute_features_action_, SIGNAL(triggered()),
          this, SLOT(ComputeFeatures()));
  
  compute_candidate_matches_action_ =
      new QAction(tr("&Compute Candidate Matches"), this);
  compute_candidate_matches_action_->setStatusTip(
      tr("Compute Candidate Matches"));
  connect(compute_candidate_matches_action_, SIGNAL(triggered()),
          this, SLOT(ComputeCandidateMatches()));
  
  compute_robust_matches_action_ = new QAction(tr("&Compute Robust Matches"),
                                               this);
  compute_robust_matches_action_->setStatusTip(tr("Compute Robust Matches"));
  connect(compute_robust_matches_action_, SIGNAL(triggered()),
          this, SLOT(ComputeRobustMatches()));

  focal_from_fundamental_action_ = new QAction(tr("&Focal from Fundamental"),
                                               this);
  focal_from_fundamental_action_->setStatusTip(
      tr("Compute Focal Distance from the Fundamental MatrixRobust Matrix"));
  connect(focal_from_fundamental_action_, SIGNAL(triggered()),
          this, SLOT(FocalFromFundamental()));
}

void TvrMainWindow::CreateMenus() {
  file_menu_ = menuBar()->addMenu(tr("&File"));
  file_menu_->addAction(open_images_action_);
  matching_menu_ = menuBar()->addMenu(tr("&Matching"));
  matching_menu_->addAction(compute_features_action_);
  matching_menu_->addAction(compute_candidate_matches_action_);
  matching_menu_->addAction(compute_robust_matches_action_);
  calibration_menu_ = menuBar()->addMenu(tr("&Calibration"));
  calibration_menu_->addAction(focal_from_fundamental_action_);
}

void TvrMainWindow::OpenImages() {
  QStringList filenames = QFileDialog::getOpenFileNames(this,
      "Select Two Images", "", "Image Files (*.png *.jpg *.bmp *.ppm *.pgm *.xpm)");

  // TODO(keir): Don't segfault if != 2 images are selected.
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
  // TODO(keir): Make a libmv image <-> QImage interop library inside libmv for
  // easy bidirectional exchange of images between Qt and libmv.
  libmv::Array3Df image(height, width, 1);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // TODO(pau): there are better ways to compute intensity.
      //            Implement one and put it on libmv/image.
      int depth = qimage.depth() / 8;
      image(y, x) = 0;
      for (int c = 0; c < depth; ++c) {
        if (c != 3) {  // Skip alpha channel for RGBA.
          image(y, x) = float(qimage.bits()[depth*(y * width + x) + c]);
        }
      }
      image(y, x) /= depth == 4 ? 3 : depth;
    }
  }
  libmv::SurfFeatures(image, 3, 4, &fs.features);

  // Build the kd-tree.
  fs.tree.Build(&fs.features[0], fs.features.size(), 64, 10);

  viewer_->updateGL();
}

void TvrMainWindow::ComputeCandidateMatches() {
  FindCandidateMatches(document_.feature_sets[0],
                       document_.feature_sets[1],
                       &document_.correspondences);
  viewer_->updateGL();
}

void TvrMainWindow::ComputeRobustMatches() {
  libmv::Correspondences new_correspondences;

  ComputeFundamental(document_.correspondences,
                     &document_.F, &new_correspondences);
  
  // TODO(pau) Make sure this is not copying too many things.  We could
  //           implement an efficient swap for the biparted graph (just swaping
  //           the maps), or remove outlier tracks from the candidate matches
  //           instead of constructing a new correspondance set.
  std::swap(document_.correspondences, new_correspondences);
  viewer_->updateGL();
}

void TvrMainWindow::FocalFromFundamental() {
  libmv::Vec2 p0(document_.images[0].width(), document_.images[0].height());
  libmv::Vec2 p1(document_.images[1].width(), document_.images[1].height());
  libmv::FocalFromFundamental(document_.F, p0, p1,
                              &document_.focal_distance[0],
                              &document_.focal_distance[1]);

  LOG(INFO) << "focal 0: " << document_.focal_distance[0]
            << " focal 1: " << document_.focal_distance[1] << "\n";

  viewer_->updateGL();
}
