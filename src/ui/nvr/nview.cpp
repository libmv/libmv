// Copyright (c) 2010 libmv authors.
// Initial revision by Matthias Fauconneau.

#include "ui/nvr/nview.h"
#include <QtGui>

#include "libmv/descriptor/descriptor_factory.h"
#include "libmv/detector/detector_factory.h"

int main(int argc, char *argv[]) {
  Init("", &argc, &argv);
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("nView");
  setMinimumSize(800, 600);

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

MainWindow::~MainWindow()
{
  DeleteElements(&images);
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
    view->setImage(images[i], &(nViewMatcher.getMatches()), i );
    gridLayout->addWidget(view,i/ratio,i%ratio);
  }
}

void MatchView::setImage( nImage* image, const libmv::Matches * matches, int index ) {
  this->image=image;
  connect(image,SIGNAL(updated()),this,SLOT(update()));

  _mMatches = matches;
  _indexInMatchesTable = index;
}

void MatchView::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.drawImage(0,0, image->scaledToWidth(width()));
  p.setTransform(QTransform::fromScale(
                   (float)width()/image->width(),
                   (float)width()/image->width()));

  p.setBrush(Qt::NoBrush);

  if (_indexInMatchesTable < _mMatches->NumImages())
  {
    libmv::Matches::Features<KeypointFeature> ptr =
      _mMatches->InImage<KeypointFeature>(_indexInMatchesTable);
    while( ptr)
    {
      const KeypointFeature* f = ptr.feature();
      p.setPen(QColor(255,255,255));
      p.drawEllipse(QPointF(f->x(), f->y()), f->scale,
                    f->scale );
      p.drawLine( QPoint(f->x(), f->y()),
                  QPoint(
                    f->x() +
                    f->scale*cos(f->orientation),
                    f->y() +
                    f->scale*sin(f->orientation)));
      ++ptr;
    }
  }
}

void nImage::update() {
  emit updated(); //Refresh display
}

void MainWindow::computeMatches() {

  libmv::vector<std::string> image_vector;
  foreach( nImage* image, images ) {
    image_vector.push_back( image->_path);
  }

  statusBar()->showMessage(
          QString("Computing matches using NViewMatcher Interface"));

  /*scoped_ptr<detector::Detector>
  //detector(detector::CreateFastDetector(9, 30,true));
  // detector(detector::CreateStarDetector(true));
  // detector(detector::CreateFastDetectorLimited(30,true, 1024));
   detector(detector::CreateMserDetector());

  scoped_ptr<descriptor::Describer>
    describer(descriptor::CreateSimpliestDescriber());
  //describer(descriptor::CreateSurfDescriber());
  //describer(descriptor::CreateDaisyDescriber());
  //describer(descriptor::CreateDipoleDescriber());
  nViewMatcher = correspondence::nRobustViewMatching(detector.get(),
                                                     describer.get());*/

  detector::Detector * pdetector  =
    detector::detectorFactory(detector::FAST_DETECTOR);
  descriptor::Describer* pdescriber =
    descriptor::describerFactory(descriptor::DIPOLE_DESCRIBER);
  nViewMatcher = correspondence::nRobustViewMatching(pdetector, pdescriber);

  nViewMatcher.computeCrossMatch(image_vector);

  //TODO(pmoulon) Update the Graph view with images that have cross matches nodes.

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
