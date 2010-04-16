// Copyright 2010 Matthias Fauconneau
#include "ui/nvr/nview.h"

int main(int argc, char *argv[]) {
  Init("", &argc, &argv);
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("nView");
  setMinimumSize(1024,768);

  QDockWidget* actionsDock = new QDockWidget("Actions",this);
  QWidget* actionsWidget = new QWidget(actionsDock);
  actionsDock->setWidget(actionsWidget);
  QVBoxLayout* layout = new QVBoxLayout(actionsWidget);
  {
    QPushButton* button = new QPushButton("Add Images...");
    connect(button,SIGNAL(clicked()),SLOT(addImages()));
    layout->addWidget(button);
  }
  {
    QPushButton* button = new QPushButton("Compute Matches...");
    connect(button,SIGNAL(clicked()),SLOT(computeMatches()));
    layout->addWidget(button);
  }
  addDockWidget(Qt::LeftDockWidgetArea,actionsDock);  // dummy

  addDockWidget(Qt::LeftDockWidgetArea,new QDockWidget("Match Graph",this));

  QTabWidget* tab = new QTabWidget(this);
  QWidget* gridView = new QWidget(tab);
  gridLayout = new QGridLayout(gridView);
  gridView->setLayout(gridLayout);
  tab->addTab( gridView, "Match View");
  tab->addTab( new QWidget(), "3D View"); //dummy
  setCentralWidget( tab );

}

void MainWindow::addImages() {
  addImages(QFileDialog::getOpenFileNames(this,
    "Select Images",
    "",
    "Pictures (*.png *.jpg *.jpeg *.bmp *.ppm *.pgm *.xpm *.tif *.tiff *.tga);\
    All Files"));
}

void MainWindow::addImages( QStringList filenames ) {

  foreach( QString filename, filenames ) {
    qDebug() << filename << QFile(filename).exists();
    if (QFile(filename).exists()) images << new nImage(filename); // Memory Leak !
  }
  int ratio = (int)ceil(sqrt((float)images.count()));
  for (int i=0;i<images.count();i++) {
    MatchView* view = new MatchView();
    view->setImage(images[i]);
    gridLayout->addWidget(view,i/ratio,i%ratio);
  }
}

void MatchView::setImage( nImage* image ) {
  this->image=image;
  connect(image,SIGNAL(updated()),this,SLOT(update()));
}

void MatchView::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.drawImage(0,0, image->scaledToWidth(width()));
  p.setTransform(QTransform::fromScale(
                   (float)width()/image->width(),
                   (float)width()/image->width()));

  p.setBrush(Qt::NoBrush);
  foreach( nMatch* match, image->allMatches().values() ) {
    for (Matches::Points r = match->features(image); r; ++r ) {
      const PointFeature* f = r.feature();
      p.setPen(QColor(255,255,255));
      p.drawEllipse(QPointF(f->x(), f->y()), r.feature()->scale,
                    r.feature()->scale );
      p.drawLine( QPoint(f->x(), f->y()),
                  QPoint(
                    f->x() +
                    r.feature()->scale*cos(r.feature()->orientation),
                    f->y() +
                    r.feature()->scale*sin(r.feature()->orientation)));
    }
  }
}

void nImage::computeFeatures() {
  Array3Du image(height(), width());
  for (int y=0; y<height(); y++) {
    QRgb* rgb = (QRgb*)scanLine(y);
    for (int x=0; x<width(); x++)
      image(y, x) = qGray(rgb[x]);
  }
  scoped_ptr<detector::Detector> detector(
    detector::CreateFastDetector(9, 30, true));


  vector<Feature *> features;
  Image im(new Array3Du(image));
  detector->Detect(im, &features, NULL);

  vector<descriptor::Descriptor *> descriptors;
  scoped_ptr<descriptor::Describer> describer(
    descriptor::CreateSimpliestDescriber());
  describer->Describe(features, im, NULL, &descriptors);

  // Copy data.
  featureSet.features.resize(descriptors.size());
  for (int i = 0;i < descriptors.size(); i++) {
    KeypointFeature& feature = featureSet.features[i];
    feature.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
    *(PointFeature*)(&feature) = *(PointFeature*)features[i];
  }

  DeleteElements(&descriptors);
  DeleteElements(&features);
}
bool nImage::hasMatch(nImage *other) {
  return matches.contains(other);
}
QMap<nImage*,nMatch*> nImage::allMatches() {
  return matches;
}
void nImage::clearMatches() {
  matches.clear();
}
void nImage::update() {
  emit updated();
}

nMatch::nMatch(nImage *A, nImage *B) : A(A), B(B) {
  FindCandidateMatches( A->featureSet, B->featureSet, &mvMatch );
  /* //WIP geometric filter
  //----------------------------------------------
  //-- Epipolar filter
  libmv::Matches robustMatches;
  // Construct matrices containing the matches.
  vector<Mat> x;
  vector<int> tracks;
  vector<int> imageIndices;
  imageIndices.push_back(0);
  imageIndices.push_back(1);

  PointMatchMatrices(this->mvMatch, imageIndices, &tracks, &x);

  // Compute Fundamental matrix and inliers.
  vector<int> inliers;
  Mat3 F;
  FundamentalFromCorrespondences7PointRobust(x[0], x[1], 1, &F, &inliers);
  // Build new correspondence graph containing only inliers.
  for (int j = 0; j < inliers.size(); ++j) {
      int k = inliers[j];
      for (int i = 0; i < 2; ++i) {
          robustMatches.Insert(i, tracks[k], this->mvMatch.Get(i, tracks[k]));
      }
  }

  this->mvMatch = robustMatches;
  //------------------------------------------
  */
  A->matches[B]=this;
  B->matches[A]=this;
}

Matches::Points nMatch::features( nImage* image ) {
  return mvMatch.InImage<PointFeature>(image==A?0:1);
}

void MainWindow::computeMatches() {
  int i=0;
  foreach( nImage* image, images ) {
    statusBar()->showMessage(
      QString("Computing features (%1/%2)").arg(i++).arg(images.count()));
    image->computeFeatures();
    QApplication::processEvents();
  }
  int a=0;
  foreach( nImage* A, images ) {
    A->clearMatches();
    int b=0;
    foreach( nImage* B, images ) {
      if ( A != B && !A->hasMatch(B) ) {
        statusBar()->showMessage(
          QString("Computing matches (%1 <-> %2)").arg(a).arg(b));
        new nMatch( A, B ); //Memory leak !
        A->update();
        B->update();
      }
      QApplication::processEvents();
      ++b;
      if (b>=a)
        break;
    }
    a++;
  }
  statusBar()->showMessage("Done.");
}

void MainWindow::focalFromFundamental() {
  // Todo
}

void MainWindow::metricReconstruction() {
  // Todo
}

void MainWindow::metricBundle() {
  // Todo
}

void MainWindow::saveBlender() {
  // Todo
}
