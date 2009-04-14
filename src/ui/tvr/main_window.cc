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
#include <QtGui>

#include "libmv/image/array_nd.h"
#include "libmv/image/surf.h"
#include "libmv/image/surf_descriptor.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/logging/logging.h"
#include "ui/tvr/main_window.h"


TvrMainWindow::TvrMainWindow(QWidget *parent)
  : QMainWindow(parent) {

  viewer2d_ = new MatchViewer();
  viewer3d_ = new Viewer3D();
  setCentralWidget(viewer2d_);
  current_view_ = view2d;

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

  toggle_view_action_ = new QAction(tr("&Toggle View"), this);
  toggle_view_action_->setShortcut(tr("TAB"));
  toggle_view_action_->setStatusTip(tr("Toggle between 2D and 3D"));
  connect(toggle_view_action_, SIGNAL(triggered()),
          this, SLOT(ToggleView()));

  save_blender_action_ = new QAction(tr("&Save as Blender..."), this);
  save_blender_action_->setShortcut(tr("Ctrl+S"));
  save_blender_action_->setStatusTip(tr("Save Scene as a Blender Script"));
  connect(save_blender_action_, SIGNAL(triggered()),
          this, SLOT(SaveBlender()));
  

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

  metric_reconstruction_action_ = new QAction(tr("&Metric Reconstruction"), this);
  metric_reconstruction_action_->setStatusTip(tr(
      "Compute a metric reconstrution given the current focal length estimates"));
  connect(metric_reconstruction_action_, SIGNAL(triggered()),
          this, SLOT(MetricReconstruction()));
}

void TvrMainWindow::CreateMenus() {
  file_menu_ = menuBar()->addMenu(tr("&File"));
  file_menu_->addAction(open_images_action_);
  file_menu_->addAction(save_blender_action_);
  view_menu_ = menuBar()->addMenu(tr("&View"));
  view_menu_->addAction(toggle_view_action_);
  matching_menu_ = menuBar()->addMenu(tr("&Matching"));
  matching_menu_->addAction(compute_features_action_);
  matching_menu_->addAction(compute_candidate_matches_action_);
  matching_menu_->addAction(compute_robust_matches_action_);
  calibration_menu_ = menuBar()->addMenu(tr("&Calibration"));
  calibration_menu_->addAction(focal_from_fundamental_action_);
  calibration_menu_->addAction(metric_reconstruction_action_);
}

void TvrMainWindow::OpenImages() {
  QStringList filenames = QFileDialog::getOpenFileNames(this,
      "Select Two Images", "", "Image Files (*.png *.jpg *.bmp *.ppm *.pgm *.xpm)");

  if (filenames.size() == 2) {
    for (int i = 0; i < 2; ++i) {
      document_.images[i].load(filenames[i]);
    }
    viewer2d_->SetDocument(&document_);
    viewer3d_->SetDocument(&document_);
  } else if (filenames.size() != 0) {
    QMessageBox::information(this, tr("TVR"),
          tr("Please select 2 images."));
    OpenImages();
  }
}

void TvrMainWindow::SaveBlender() {
  QString filename = QFileDialog::getSaveFileName(this,
      "Save as Blender Script", "", "Blender Python Script (*.py)");
  if(filename.isNull())
    return;
  document_.SaveAsBlender(filename.toAscii().data());
}

void TvrMainWindow::ToggleView() {
  if(current_view_ == view2d) {
    setCentralWidget(viewer3d_);
    current_view_ = view3d;
  } else {
    setCentralWidget(viewer2d_);
    current_view_ = view2d;
  }
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

  viewer2d_->updateGL();
}

void TvrMainWindow::ComputeCandidateMatches() {
  FindCandidateMatches(document_.feature_sets[0],
                       document_.feature_sets[1],
                       &document_.correspondences);
  viewer2d_->updateGL();
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
  viewer2d_->updateGL();
}

void TvrMainWindow::FocalFromFundamental() {
  libmv::Vec2 p0((document_.images[0].width() - 1) / 2.,
                 (document_.images[0].height() - 1) / 2.);
  libmv::Vec2 p1((document_.images[1].width() - 1) / 2.,
                 (document_.images[1].height() - 1) / 2.);
  libmv::FocalFromFundamental(document_.F, p0, p1,
                              &document_.focal_distance[0],
                              &document_.focal_distance[1]);

  LOG(INFO) << "focal 0: " << document_.focal_distance[0]
            << " focal 1: " << document_.focal_distance[1] << "\n";

  viewer2d_->updateGL();
}

void TvrMainWindow::MetricReconstruction() {
  using namespace libmv;

  Vec2 p0((document_.images[0].width() - 1) / 2.,
          (document_.images[0].height() - 1) / 2.);
  Vec2 p1((document_.images[1].width() - 1) / 2.,
          (document_.images[1].height() - 1) / 2.);
  double f0 = document_.focal_distance[0];
  double f1 = document_.focal_distance[1];
  Mat3 K0, K1;
  K0 << f0,  0, p0(0),
         0, f0, p0(1),
         0,  0,     1;
  K1 << f1,  0, p1(0),
         0, f1, p1(1),
         0,  0,     1;
  
  // Compute essential matrix
  Mat3 E;
  EssentialFromFundamental(document_.F, K0, K1, &E);

  // Get one match from the corresponcence structure.
  Vec2 x[2];
  {
    Correspondences::TrackIterator t =
        document_.correspondences.ScanAllTracks();
    PointCorrespondences::Iterator it =
        PointCorrespondences(&document_.correspondences)
              .ScanFeaturesForTrack(t.track());
    x[it.image()](0) = it.feature()->x();
    x[it.image()](1) = it.feature()->y();
    it.Next();
    x[it.image()](0) = it.feature()->x();
    x[it.image()](1) = it.feature()->y();
  }

  // Recover R, t from E and K
  Mat3 R;
  Vec3 t;
  MotionFromEssentialAndCorrespondence(E, K0, x[0], K1, x[1], &R, &t);

  LOG(INFO) << "R:\n" << R << "\nt:\n" << t;

  document_.K[0] = K0;
  document_.R[0] = Mat3::Identity();
  document_.t[0] = Vec3::Zero();
  document_.K[1] = K1;
  document_.R[1] = R;
  document_.t[1] = t;

  // Triangulate features.
  {
    std::vector<Mat34> Ps(2);
    P_From_KRt(document_.K[0], document_.R[0], document_.t[0], &Ps[0]);
    P_From_KRt(document_.K[1], document_.R[1], document_.t[1], &Ps[1]);
    int n = document_.correspondences.NumTracks();
    document_.X.resize(n);
    int i = 0;
    for (Correspondences::TrackIterator t =
             document_.correspondences.ScanAllTracks(); !t.Done(); t.Next()) {
      PointCorrespondences::Iterator it =
          PointCorrespondences(&document_.correspondences)
              .ScanFeaturesForTrack(t.track());
      Mat2X x(2, 2);
      Vec4 X;
      x(0, it.image()) = it.feature()->x();
      x(1, it.image()) = it.feature()->y();
      it.Next();
      x(0, it.image()) = it.feature()->x();
      x(1, it.image()) = it.feature()->y();
      NViewTriangulate(x, Ps, &X);
      X /= X(3);
      document_.X[i] = X.start<3>();
      i++;
    }
  }
}
