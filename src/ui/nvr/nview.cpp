// Copyright (c) 2010 libmv authors.
// Initial revision by Matthias Fauconneau.
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

void ImageView::setFeatures(QVector<QVector<KeypointFeature> > f) { features=f; }
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
        QPushButton* button = new QPushButton("Open Images...");
        connect(button,SIGNAL(clicked()),SLOT(openImages()));
        layout->addWidget(button);
    }
    {
        QPushButton* button = new QPushButton("Compute Matches...");
        connect(button,SIGNAL(clicked()),SLOT(computeMatches()));
        layout->addWidget(button);
    }
    {
        QPushButton* button = new QPushButton("Reconstruction...");
        connect(button,SIGNAL(clicked()),SLOT(computeReconstruction()));
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
  reconstruct.ClearCamerasMap();
  reconstruct.ClearStructuresMap(); 
}

void MainWindow::openImages() {
    openImages(QFileDialog::getOpenFileNames(
            this,"Select Images","",
            "Pictures (*.png *.jpg *.jpeg *.bmp *.ppm *.pgm *.xpm *.tif *.tiff *.tga);All Files"));
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

QProgressDialog* currentProgress;
bool progressCallback(int progress) {
    currentProgress->setValue(progress);
    return currentProgress->wasCanceled();
}

void MainWindow::computeMatches() {
    QProgressDialog progress("Computing matches...","Abort", 0, images.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    currentProgress = &progress;

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

    Detector * pdetector  = detectorFactory(FAST_DETECTOR);
    Describer* pdescriber = describerFactory(DIPOLE_DESCRIBER);
    nViewMatcher = correspondence::nRobustViewMatching(pdetector, pdescriber);

    libmv::vector<std::string> image_vector;
    foreach (ImageView* image, images) image_vector.push_back( image->path().toStdString() );
    nViewMatcher.computeCrossMatch(image_vector);

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
            for(Matches::Features<KeypointFeature> features=
                nViewMatcher.getMatches().InImage<KeypointFeature>(i);features;++features) {
                Matches::TrackID id_track = features.track();
                const KeypointFeature * ref = features.feature();
                const KeypointFeature * f = (KeypointFeature*) //FIXME: could Get() return FeatureT ?
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
    }
    foreach(Edge* e, graph->edges )
        e->setPen(QPen(QBrush(Qt::SolidPattern),0.1*e->pen().width()/maxWidth));
    for (int i=0; i < images.count(); ++i) images[i]->setFeatures(matches[i]);
    graphView->fitInView(graph->sceneRect());
}

void MainWindow::computeReconstruction() {
  QProgressDialog progress("Computing reconstruction...","Abort", 0,
                           images.count(),this);
  progress.setWindowModality(Qt::WindowModal);
  currentProgress = &progress;
  Matches &matches = nViewMatcher.getMatches();
  size_t index_image = 0;
  Vec2u image_size;
  PinholeCamera * camera = NULL;
  std::set<Matches::ImageID>::const_iterator image_iter =
    matches.get_images().begin();
  Matches::ImageID previous_image_id = *image_iter;
    
  if (images.count()<2)
    return;
  // Estimation of the second image
  image_iter++;
  index_image++;
  
  progress.setLabelText("Initial Motion Estimation");
  LOG(INFO) << " -- Initial Motion Estimation --  " << std::endl;
  ReconstructFromTwoUncalibratedViews(matches, previous_image_id, *image_iter,
                                      &matches, &reconstruct);
  camera = dynamic_cast<PinholeCamera*>(
    reconstruct.GetCamera(*(matches.get_images().begin())));
  if (camera) {
    image_size << images[0]->width(), images[0]->height();
    camera->set_image_size(image_size); 
  }
  camera = dynamic_cast<PinholeCamera*>(reconstruct.GetCamera(*image_iter));
  if (camera) {
    image_size << images[1]->width(), images[1]->height();
    camera->set_image_size(image_size); 
  }
  progressCallback(2);
  
  progress.setLabelText("Initial Intersection");
  LOG(INFO) << " -- Initial Intersection --  " << std::endl;
  size_t minimum_num_views = 2;
  PointStructureTriangulation(matches, *image_iter, minimum_num_views, 
                              &reconstruct);

  //LOG(INFO) << " -- Bundle Adjustment --  " << std::endl;
  //TODO (julien) Perfom Bundle Adjustment (Euclidean BA)
  
  // Estimation of the pose of other images by resection
  image_iter++;
  index_image++;
  for (; image_iter != matches.get_images().end();
       ++image_iter, ++index_image) {
    
    progress.setLabelText("Incremental Resection");
    LOG(INFO) << " -- Incremental Resection --  " << std::endl;
    UncalibratedCameraResection(matches, *image_iter,
                                &matches, &reconstruct);     
    
    camera = dynamic_cast<PinholeCamera*>(reconstruct.GetCamera(*image_iter));
    if (camera) {
      image_size << images[index_image]->width(), images[index_image]->height();
      camera->set_image_size(image_size); 
    }
    progressCallback(index_image);
    progress.setLabelText("Incremental Intersection");
    LOG(INFO) << " -- Incremental Intersection --  " << std::endl;
    size_t minimum_num_views = 3;
    PointStructureTriangulation(matches, 
                                *image_iter,
                                minimum_num_views, 
                                &reconstruct);
    
    //LOG(INFO) << " -- Bundle Adjustment --  " << std::endl;
    //TODO (julien) Perfom Bundle Adjustment (Euclidean BA)
  }
  progress.setLabelText("Metric rectification");
  // Metric rectification
  UpgradeToMetric(matches, &reconstruct);
  progress.setLabelText("Reconstruction done.");
  ExportToPLY(reconstruct, "./out.ply");
}
/// Graph
Graph::Graph() : QGraphicsScene() { startTimer(40); }

void Graph::evaluate( float dt, int current ) {
    if( current > 0 ) { //euler integration from initial state using previous derivative
        foreach( Node* p, nodes ) {
            p->position[current] = p->position[0] + dt * p->velocity[current-1];
            p->velocity[current] = p->velocity[0] + dt * p->acceleration[current-1];
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
        p->position[0] += 1/6.0 * 0.04 * ( p->velocity[0] + 2*p->velocity[1] + 2*p->velocity[2] + p->velocity[3] );
        p->velocity[0] += 1/6.0 * 0.04 * ( p->acceleration[0] + 2*p->acceleration[1] + 2*p->acceleration[2] + p->acceleration[3] );
        p->setPos(p->position[0].toPointF());
    }
    foreach( Edge* e, edges ) e->setLine(QLineF(e->a->pos(),e->b->pos()));
    /*TODO:
      if(!itemsMoved && qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1) killTimer(e->timerId());
      void GraphWidget::itemMoved() { startTimer(40); }
    */
}
