// Copyright (c) 2010 libmv authors.
// Initial revision by Matthias Fauconneau.
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include "ui/nvr/nview.h"
#include <QLabel>

int main(int argc, char *argv[]) {
    Init("", &argc, &argv);
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

/// ImageView

void ImageView::setFeatures(QVector<QVector<KeypointFeature> > f) { features=f;
}
void ImageView::setFilter(int i) { filter=i; update(); }
void ImageView::clearFilter() { filter=-1; update(); }
void ImageView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.scale((float)width()/image.width(),(float)width()/image.width());
    p.drawPixmap(0,0, image);
    p.setPen(Qt::white);
    for (int i=0; i<features.count(); i++ ) {
        if( filter>=0 && i != filter && index != filter ) continue;
        foreach(KeypointFeature f, features[i] ) {
            p.drawEllipse(QPointF(f.x(), f.y()), f.scale, f.scale );
            p.drawLine(QPoint(f.x(), f.y()),
                       QPoint( f.x() + f.scale*cos(f.orientation),
                               f.y() + f.scale*sin(f.orientation) ) );
        }
    }
}

void ImageView::enterEvent(QEvent*) { emit enter(index); }
void ImageView::leaveEvent(QEvent*) { emit leave(); }

/// MainWindow

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("nView");
    setMinimumSize(800, 600);

    QDockWidget* actionsDock = new QDockWidget("Actions",this);
    QWidget* actionsWidget = new QWidget(actionsDock);
    actionsDock->setWidget(actionsWidget);
    QVBoxLayout* layout = new QVBoxLayout(actionsWidget);
    {
      QPushButton* button = new QPushButton("Open Images");
      connect(button,SIGNAL(clicked()),SLOT(openImages()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Compute Matches (images)");
      connect(button,SIGNAL(clicked()),SLOT(computeMatches()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Compute Relative Matches (video)");
      connect(button,SIGNAL(clicked()),SLOT(computeRelativeMatches()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("*Uncalibrated Reconstruction*");
      connect(button,SIGNAL(clicked()),
        SLOT(computeUncalibratedReconstruction()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Metric Rectification");
      connect(button,SIGNAL(clicked()),SLOT(computeMetricRectification()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("*Calibrated Reconstruction*");
      connect(button,SIGNAL(clicked()),
        SLOT(computeCalibratedReconstruction()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Metric Bundle Adjustment");
      connect(button,SIGNAL(clicked()),SLOT(computeBA()));
      layout->addWidget(button);
    }
    addDockWidget(Qt::LeftDockWidgetArea,actionsDock);

    QDockWidget* graphDock = new QDockWidget("Match Graph",this);
    graph = new Graph;
    graphView = new GraphView;
    graphView->setScene(graph);
    graphView->setRenderHint(QPainter::Antialiasing);
    graphView->setDragMode(QGraphicsView::ScrollHandDrag);
    graphView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    graphDock->setWidget(graphView);
    addDockWidget(Qt::LeftDockWidgetArea,graphDock);

    QTabWidget* tab = new QTabWidget(this);
    QWidget* gridView = new QWidget(tab);
    gridLayout = new QGridLayout(gridView);
    gridView->setLayout(gridLayout);
    tab->addTab( gridView, "Match View");

    tab->addTab( new QWidget(), "3D View");
    setCentralWidget( tab );

    QStringList args = qApp->arguments(); args.removeFirst();
    openImages(args);
    if(!args.isEmpty()) computeMatches();
}

MainWindow::~MainWindow() {
  reconstruction_.ClearCamerasMap();
  reconstruction_.ClearStructuresMap(); 
}

void MainWindow::openImages() {
    openImages(QFileDialog::getOpenFileNames(
            this,"Select Images","",
	    "Pictures (*.png *.jpg *.jpeg *.bmp *.ppm *.pgm *.xpm *.tif *.tiff*.tga);All Files"));
}

void MainWindow::openImages( QStringList files ) {
    if(files.isEmpty()) return;
    foreach(ImageView* w,images) {
        gridLayout->removeWidget(w);
        delete w;
    }
    images.clear();
    int ratio = (int)ceil(sqrt((float)files.count()));
    for(int i=0;i<files.count();i++ ) {
        ImageView* view = new ImageView(i,files[i]);
        connect(view,SIGNAL(enter(int)),SIGNAL(setFilter(int)));
        connect(view,SIGNAL(leave()),SIGNAL(clearFilter()));
        connect(this,SIGNAL(setFilter(int)),view,SLOT(setFilter(int)));
        connect(this,SIGNAL(clearFilter()),view,SLOT(clearFilter()));
        gridLayout->addWidget(view,i/ratio,i%ratio);
        images << view;
    }
}

bool progressCallback(QProgressDialog &progress, int n) {
    progress.setValue(n);
    return progress.wasCanceled();
}
void MainWindow::warningNotFunctional() {     
  QMessageBox::warning (this, "Warning", 
                        "This process is STILL in development.");
}

bool MainWindow::SelectDetectorDescriber(eDetector detector, 
                                         eDescriber describer) {

  // Set the detector
  std::map<std::string, detector::eDetector> detectorMap;
  detectorMap["FAST"] = detector::FAST_DETECTOR;
  detectorMap["SURF"] = detector::SURF_DETECTOR;
  detectorMap["STAR"] = detector::STAR_DETECTOR;
  detectorMap["MSER"] = detector::MSER_DETECTOR;
  
  // Set the descriptor
  std::map<std::string, descriptor::eDescriber> descriptorMap;
  descriptorMap["SIMPLIEST"] = descriptor::SIMPLEST_DESCRIBER;
  descriptorMap["SURF"]      = descriptor::SURF_DESCRIBER;
  descriptorMap["DIPOLE"]    = descriptor::DIPOLE_DESCRIBER;
  descriptorMap["DAISY"]     = descriptor::DAISY_DESCRIBER;
    
  QStringList detector_list;
  detector_list << tr("FAST") << tr("SURF") 
                << tr("STAR") << tr("MSER");
  
  QStringList descriptor_list;
  descriptor_list << tr("SIMPLIEST") << tr("SURF") 
                  << tr("DIPOLE") << tr("DAISY");
  
  bool ok;
  QString item = QInputDialog::getItem(this, tr("Choose a detector..."),
                                        tr("Detector:"), 
                                        detector_list, 0, false, &ok);
  if (ok && !item.isEmpty())    
    detector = detectorMap[item.toStdString()];
  item = QInputDialog::getItem(this, tr("Choose a describer..."),
                                        tr("Descriptor:"), 
                                        descriptor_list, 3, false, &ok);
  if (ok && !item.isEmpty())    
    describer = descriptorMap[item.toStdString()];
  
  return ok;
}

void MainWindow::computeMatches() {
    QProgressDialog progress("Computing matches...","Abort", 0, 
                             images.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    eDetector detector   = detector::FAST_DETECTOR;
    eDescriber describer = descriptor::DAISY_DESCRIBER;
    SelectDetectorDescriber(detector, describer);
    // TODO(julien) create a UI to selection the detector/describer we want
    Detector * pdetector  = detectorFactory(detector);
    Describer* pdescriber = describerFactory(describer);
    nViewMatcher = correspondence::nRobustViewMatching(pdetector, pdescriber);

    libmv::vector<std::string> image_vector;
    foreach (ImageView* image, images) 
      image_vector.push_back(image->path().toStdString());
    nViewMatcher.computeCrossMatch(image_vector);

    // TODO(julien) put the following in a private function
    graph->clear(); graph->nodes.clear(); graph->edges.clear(); //leak?
    QVector< QVector< QVector<KeypointFeature> > > matches;
    matches.resize(images.count());
    for (int i=0; i < images.count(); ++i) matches[i].resize(images.count());
    int maxWidth=0;
    for (int i=0; i < images.count(); ++i) {
        Node* a = new Node;
        a->setFlag(QGraphicsItem::ItemIsMovable);
        a->setPixmap(images[i]->image);
        a->setScale(1.0/a->pixmap().width());
        a->setOffset(-a->pixmap().width()/2,-a->pixmap().height()/2);
        a->setPos( (float)qrand()/INT_MAX, (float)qrand()/INT_MAX );
        graph->addItem(a);
        graph->nodes << a;
        for (int j=0; j<i; ++j)  {
            Node* b = graph->nodes[j];
            for(Matches::Features<KeypointFeature> features = 
                  nViewMatcher.getMatches().InImage<KeypointFeature>(i);
                  features;++features) {
                Matches::TrackID id_track = features.track();
                const KeypointFeature * ref = features.feature();
                const KeypointFeature * f = (KeypointFeature*) 
                //FIXME: could Get() return FeatureT ?
                nViewMatcher.getMatches().Get(j, id_track);
                if (f && ref) {
                    matches[i][j].append( *ref );
                    matches[j][i].append( *f );
                    if(a->edges.contains(b)) {
                        Edge* e = a->edges[b];
                        int width = e->pen().width()+1;
                        maxWidth = qMax(maxWidth,width);
                        e->setPen(QPen(QBrush(Qt::SolidPattern),width));
                    } else {
                        Edge* e = new Edge(a,b);
                        a->edges[b] = b->edges[a] = e;
                        graph->edges << e;
                        graph->addItem( e );
                    }
                }
            }
        }
      progressCallback(progress, i);
    }
    foreach(Edge* e, graph->edges )
        e->setPen(QPen(QBrush(Qt::SolidPattern),0.1*e->pen().width()/maxWidth));
    for (int i=0; i < images.count(); ++i) images[i]->setFeatures(matches[i]);
    graphView->fitInView(graph->sceneRect());
}

void MainWindow::computeRelativeMatches() {
    QProgressDialog progress("Computing relative matches...","Abort", 0, 
                             images.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    eDetector detector   = detector::FAST_DETECTOR;
    eDescriber describer = descriptor::DAISY_DESCRIBER;
    SelectDetectorDescriber(detector, describer);
    // TODO(julien) create a UI to selection the detector/describer we want
    Detector * pdetector  = detectorFactory(detector);
    Describer* pdescriber = describerFactory(describer);
    nViewMatcher = correspondence::nRobustViewMatching(pdetector, pdescriber);

    libmv::vector<std::string> image_vector;
    foreach (ImageView* image, images) 
      image_vector.push_back(image->path().toStdString());
    nViewMatcher.computeRelativeMatch(image_vector);

    // TODO(julien) put the following in a private function
    graph->clear(); graph->nodes.clear(); graph->edges.clear(); //leak?
    QVector< QVector< QVector<KeypointFeature> > > matches;
    matches.resize(images.count());
    for (int i=0; i < images.count(); ++i) matches[i].resize(images.count());
    int maxWidth=0;
    for (int i=0; i < images.count(); ++i) {
        Node* a = new Node;
        a->setFlag(QGraphicsItem::ItemIsMovable);
        a->setPixmap(images[i]->image);
        a->setScale(1.0/a->pixmap().width());
        a->setOffset(-a->pixmap().width()/2,-a->pixmap().height()/2);
        a->setPos( (float)qrand()/INT_MAX, (float)qrand()/INT_MAX );
        graph->addItem(a);
        graph->nodes << a;
        for (int j=0; j<i; ++j)  {
            Node* b = graph->nodes[j];
            for(Matches::Features<KeypointFeature> features = 
                  nViewMatcher.getMatches().InImage<KeypointFeature>(i);
                  features;++features) {
                Matches::TrackID id_track = features.track();
                const KeypointFeature * ref = features.feature();
                const KeypointFeature * f = (KeypointFeature*) 
                //FIXME: could Get() return FeatureT ?
                nViewMatcher.getMatches().Get(j, id_track);
                if (f && ref) {
                    matches[i][j].append( *ref );
                    matches[j][i].append( *f );
                    if(a->edges.contains(b)) {
                        Edge* e = a->edges[b];
                        int width = e->pen().width()+1;
                        maxWidth = qMax(maxWidth,width);
                        e->setPen(QPen(QBrush(Qt::SolidPattern),width));
                    } else {
                        Edge* e = new Edge(a,b);
                        a->edges[b] = b->edges[a] = e;
                        graph->edges << e;
                        graph->addItem( e );
                    }
                }
            }
        }
      progressCallback(progress, i);
    }
    foreach(Edge* e, graph->edges )
        e->setPen(QPen(QBrush(Qt::SolidPattern),0.1*e->pen().width()/maxWidth));
    for (int i=0; i < images.count(); ++i) images[i]->setFeatures(matches[i]);
    graphView->fitInView(graph->sceneRect());
}

void MainWindow::computeUncalibratedReconstruction() {
  QProgressDialog progress("Computing uncalibrated reconstruction...","Abort",0,
                           images.count(), this);
  progress.setWindowModality(Qt::WindowModal);
  Matches &matches = nViewMatcher.getMatches();
  Vec2u image_size;
  PinholeCamera * camera = NULL;
  
  if (images.count()<2)
    return;
  // Estimation of the second image
 
  progress.setLabelText("Selecting best initial images...");
  std::list<libmv::vector<Matches::ImageID> > connected_graph_list;
  SelectBestImageReconstructionOrder(matches,  &connected_graph_list);
  
  size_t image_id = 0;
  size_t index_image_graph = 0;
  std::list<libmv::vector<Matches::ImageID> >::iterator graph_iter =
    connected_graph_list.begin();
  for (; graph_iter != connected_graph_list.end(); ++graph_iter) {
    progress.setLabelText("Initial Motion Estimation");
    LOG(INFO) << " -- Initial Motion Estimation --  " << std::endl;
    ReconstructFromTwoUncalibratedViews(matches, 
                                        (*graph_iter)[0], 
                                        (*graph_iter)[1], 
                                        &matches_inliers_, 
                                        &reconstruction_);
    index_image_graph = 0;
    image_id = (*graph_iter)[index_image_graph];
    camera = dynamic_cast<PinholeCamera*>(
      reconstruction_.GetCamera(image_id));
    if (camera) {
      image_size << images[image_id]->GetImageWidth(), 
                    images[image_id]->GetImageHeight();
      camera->set_image_size(image_size); 
    }
    index_image_graph = 1;
    image_id = (*graph_iter)[index_image_graph];
    camera = dynamic_cast<PinholeCamera*>(
      reconstruction_.GetCamera(image_id));
    if (camera) {
      image_size << images[image_id]->GetImageWidth(), 
                    images[image_id]->GetImageHeight();
      camera->set_image_size(image_size); 
    }
    progressCallback(progress, 1);
    
    progress.setLabelText("Initial Intersection");
    LOG(INFO) << " -- Initial Intersection --  " << std::endl;
    size_t minimum_num_views = 2;
    PointStructureTriangulation(matches_inliers_, 
                                image_id,
                                minimum_num_views, 
                                &reconstruction_);

    // Performs projective bundle adjustment
    //LOG(INFO) << " -- Projective Bundle Adjustment --  " << std::endl;
    
    // Estimation of the pose of other images by resection
    minimum_num_views = 3;
    for (index_image_graph = 2; index_image_graph < graph_iter->size();
        ++index_image_graph) {
      image_id = (*graph_iter)[index_image_graph];
      progress.setLabelText("Incremental Resection");
      LOG(INFO) << " -- Incremental Resection --  " << std::endl;
      UncalibratedCameraResection(matches, image_id,
                                  &matches_inliers_, &reconstruction_);     
      
      camera = dynamic_cast<PinholeCamera*>(
        reconstruction_.GetCamera(image_id));
      if (camera) {
        image_size << images[image_id]->GetImageWidth(),
                      images[image_id]->GetImageHeight();
        camera->set_image_size(image_size); 
      }
      // TODO(julien) Avoid to retriangulate, prefer projective BA
      progress.setLabelText("Retriangulation");
      LOG(INFO) << " -- Retriangulation --  " << std::endl;
      PointStructureRetriangulation(matches_inliers_, 
                                  image_id,
                                  &reconstruction_);
      
      progress.setLabelText("Incremental Intersection");
      LOG(INFO) << " -- Incremental Intersection --  " << std::endl;
      PointStructureTriangulation(matches_inliers_, 
                                  image_id,
                                  minimum_num_views, 
                                  &reconstruction_);

      // TODO(julien) Performs projective bundle adjustment
      progressCallback(progress, index_image_graph);
    }
  }
}

void MainWindow::computeCalibratedReconstruction() {
  QProgressDialog progress("Computing calibrated reconstruction...","Abort", 0,
                           images.count(), this);
  progress.setWindowModality(Qt::WindowModal);
  Matches &matches = nViewMatcher.getMatches();
  Vec2u image_size;
  PinholeCamera * camera = NULL;
  
  if (images.count()<2)
    return;
  
  double focal = 505;
  double cu = images[0]->GetImageWidth()/2 - 0.5, cv = images[0]->GetImageHeight()/2 - 0.5;
  bool ok;
    
  //HACK(julien) create a better UI...
  focal = QInputDialog::getDouble(this, 
				  tr("Set the focal length (in pixels)"),
                                  tr("Focal:"), focal, 0, 10000, 4, &ok);
  if (!ok) return;
  cu = QInputDialog::getDouble(this, 
			       tr("Set the principal point coordinate (x)"),
                               tr("Principal point coordinate x:"), cu, 0, 10000, 4, &ok);
  if (!ok) return;
  cv = QInputDialog::getDouble(this, 
			       tr("Set the principal point coordinate (y)"),
                               tr("Principal point coordinate y:"), cv, 0, 10000, 4, &ok);
  if (!ok) return;
  
  Mat3 K;
  K << focal, 0, cu,
       0, focal, cv,
       0,   0,   1;
 
  progress.setLabelText("Selecting best initial images...");
  std::list<libmv::vector<Matches::ImageID> > connected_graph_list;
  SelectBestImageReconstructionOrder(matches,  &connected_graph_list);
  
  std::cout << " List order:";
  for (size_t i = 0; i < connected_graph_list.begin()->size(); ++i) {
    std::cout << (*connected_graph_list.begin())[i] << " ";
  }
  std::cout << std::endl;
  
  size_t image_id = 0;
  size_t index_image_graph = 0;
  std::list<libmv::vector<Matches::ImageID> >::iterator graph_iter =
    connected_graph_list.begin();
  for (; graph_iter != connected_graph_list.end(); ++graph_iter) {
    progress.setLabelText("Initial Motion Estimation");
    LOG(INFO) << " -- Initial Motion Estimation --  " << std::endl;
    ReconstructFromTwoCalibratedViews(matches, 
                                      (*graph_iter)[0], 
                                      (*graph_iter)[1], 
                                      K, K,
                                      &matches_inliers_, 
                                      &reconstruction_);
    index_image_graph = 0;
    image_id = (*graph_iter)[index_image_graph];
    camera = dynamic_cast<PinholeCamera*>(
      reconstruction_.GetCamera(image_id));
    if (camera) {
      image_size << images[image_id]->GetImageWidth(), images[image_id]->GetImageHeight();
      camera->set_image_size(image_size); 
    }
    index_image_graph = 1;
    image_id = (*graph_iter)[index_image_graph];
    camera = dynamic_cast<PinholeCamera*>(
      reconstruction_.GetCamera(image_id));
    if (camera) {
      image_size << images[image_id]->GetImageWidth(), images[image_id]->GetImageHeight();
      camera->set_image_size(image_size); 
    }
    progressCallback(progress, 1);
    
    progress.setLabelText("Initial Intersection");
    LOG(INFO) << " -- Initial Intersection --  " << std::endl;
    size_t minimum_num_views = 2;
    PointStructureTriangulation(matches_inliers_, 
                                image_id,
                                minimum_num_views, 
                                &reconstruction_);

    // Performs projective bundle adjustment
    LOG(INFO) << " -- Bundle adjustment --  " << std::endl;
    progress.setLabelText("Bundle adjustment");
    // Performs bundle adjustment
    BundleAdjust(matches_inliers_, &reconstruction_);
    
    // Estimation of the pose of other images by resection
    minimum_num_views = 3;
    for (index_image_graph = 2; index_image_graph < graph_iter->size();
        ++index_image_graph) {
      image_id = (*graph_iter)[index_image_graph];
      progress.setLabelText("Incremental Resection");
      LOG(INFO) << " -- Incremental Resection --  " << std::endl;
      CalibratedCameraResection(matches, image_id, K,
                                &matches_inliers_, &reconstruction_);     
      
      camera = dynamic_cast<PinholeCamera*>(
        reconstruction_.GetCamera(image_id));
      if (camera) {
        image_size << images[image_id]->GetImageWidth(),
                      images[image_id]->GetImageHeight();
        camera->set_image_size(image_size); 
      }
      
      progress.setLabelText("Incremental Intersection");
      LOG(INFO) << " -- Incremental Intersection --  " << std::endl;
      PointStructureTriangulation(matches_inliers_, 
                                  image_id,
                                  minimum_num_views, 
                                  &reconstruction_);

      // Performs bundle adjustment
      LOG(INFO) << " -- Bundle adjustment --  " << std::endl;
      progress.setLabelText("Bundle adjustment");
      BundleAdjust(matches_inliers_, &reconstruction_);
  
      progressCallback(progress, index_image_graph);
    }
  }
}

void MainWindow::computeMetricRectification() {
  warningNotFunctional();
  QProgressDialog progress("Computing reconstruction...","Abort", 0,
                           images.count(),this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setLabelText("Metric rectification");
  // Metric rectification
  UpgradeToMetric(matches_inliers_, &reconstruction_);
  ExportToPLY(reconstruction_, "out.ply");
}

void MainWindow::computeBA() {
  QProgressDialog progress("Computing reconstruction...","Abort", 0,
                           images.count(),this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setLabelText("Bundle adjustment");
  // Performs bundle adjustment
  BundleAdjust(matches_inliers_, &reconstruction_);
  progress.setLabelText("Reconstruction done.");
  ExportToPLY(reconstruction_, "out.ply");
}

/// Graph
Graph::Graph() : QGraphicsScene() { startTimer(40); }

void Graph::evaluate( float dt, int current ) {
    if( current > 0 ) { 
      //euler integration from initial state using previous derivative
        foreach( Node* p, nodes ) {
            p->position[current] = p->position[0] + dt * p->velocity[current-1];
            p->velocity[current] = p->velocity[0] + dt *
	    p->acceleration[current-1];
        }
    }
    foreach( Node* a, nodes ) {
        QVector2D position = a->position[current];
        QVector2D velocity = a->velocity[current];
        QVector2D F;
        foreach(Node* b, a->edges.keys()) { //spring forces
            QVector2D edge = b->position[current] - position;
            float lengthDelta = edge.length() - 0.1/a->edges[b]->pen().widthF();
            QVector2D spring = lengthDelta * edge.normalized();
            QVector2D damping = b->velocity[current] - velocity;
            F += spring+damping;
        }
        a->acceleration[current]=F;
    }
}

/// RK4 integrator
void Graph::timerEvent(QTimerEvent* ) {
    foreach( Node* p, nodes ) p->position[0] = QVector2D(p->pos());
    //evaluate forces in 4 points
    evaluate( 0.00, 0 );
    evaluate( 0.02, 1 );
    evaluate( 0.02, 2 );
    evaluate( 0.04, 3 );
    //integrate
    foreach( Node* p, nodes ) {
        p->position[0] += 1/6.0 * 0.04 * (p->velocity[0] 
	  + 2*p->velocity[1] + 2*p->velocity[2] + p->velocity[3]);
        p->velocity[0] += 1/6.0 * 0.04 * ( p->acceleration[0] 
	  + 2*p->acceleration[1] + 2*p->acceleration[2] + p->acceleration[3]);
        p->setPos(p->position[0].toPointF());
    }
    foreach( Edge* e, edges ) e->setLine(QLineF(e->a->pos(),e->b->pos()));
    /*TODO:
      if(!itemsMoved && qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1) killTimer(e->timerId());
      void GraphWidget::itemMoved() { startTimer(40); }
    */
}
