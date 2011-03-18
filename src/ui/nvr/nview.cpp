// Copyright (c) 2010 libmv authors.
// Initial revision by Matthias Fauconneau.
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QLabel>

#include "libmv/correspondence/ArrayMatcher_Kdtree.h"
#include "libmv/correspondence/export_matches_txt.h"
#include "libmv/correspondence/import_matches_txt.h"
#include "libmv/correspondence/robust_tracker.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_converter.h"
#include "libmv/logging/logging.h"
#include "libmv/reconstruction/euclidean_reconstruction.h"
#include "libmv/reconstruction/export_blender.h"
#include "libmv/reconstruction/export_ply.h"
#include "libmv/reconstruction/image_selection.h"
#include "libmv/reconstruction/keyframe_selection.h"
#include "libmv/reconstruction/mapping.h"
#include "libmv/reconstruction/optimization.h"
#include "libmv/reconstruction/projective_reconstruction.h"
#include "ui/nvr/nview.h"

int main(int argc, char *argv[]) {
  // TODO(julien) find why InitGoogleLogging prevent logging???
  //Init("", &argc, &argv);
  //google::InitGoogleLogging((argv)[0]);
  google::SetUsageMessage("eee");
  google::ParseCommandLineFlags(&argc, &argv, true);
  
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
      connect(button,SIGNAL(clicked()),SLOT(OpenImages()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Compute Matches (images)");
      connect(button,SIGNAL(clicked()),SLOT(ComputeMatches()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Compute Relative Matches (video)");
      connect(button,SIGNAL(clicked()),SLOT(ComputeRelativeMatches()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Load Matches");
      connect(button,SIGNAL(clicked()),SLOT(LoadMatches()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("*Uncalibrated Reconstruction*");
      connect(button,SIGNAL(clicked()),
        SLOT(ComputeUncalibratedReconstruction()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Metric Rectification");
      connect(button,SIGNAL(clicked()),SLOT(ComputeMetricRectification()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("*Calibrated Reconstruction*");
      connect(button,SIGNAL(clicked()),
        SLOT(ComputeCalibratedReconstruction()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Metric Bundle Adjustment");
      connect(button,SIGNAL(clicked()),SLOT(ComputeBA()));
      layout->addWidget(button);
    }
    {
      QPushButton* button = new QPushButton("Save Reconstruction");
      connect(button,SIGNAL(clicked()),SLOT(SaveReconstructionFile()));
      layout->addWidget(button);
    }
    addDockWidget(Qt::LeftDockWidgetArea,actionsDock);

    QDockWidget* graphDock = new QDockWidget("Match Graph",this);
    graph = new Graph;
    graph_view_ = new GraphView;
    graph_view_->setScene(graph);
    graph_view_->setRenderHint(QPainter::Antialiasing);
    graph_view_->setDragMode(QGraphicsView::ScrollHandDrag);
    graph_view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    graphDock->setWidget(graph_view_);
    addDockWidget(Qt::LeftDockWidgetArea,graphDock);

    QTabWidget* tab = new QTabWidget(this);
    QWidget* gridView = new QWidget(tab);
    grid_layout_ = new QGridLayout(gridView);
    gridView->setLayout(grid_layout_);
    tab->addTab( gridView, "Match View");

    gl_widget_ = new GLWidget(this);
    tab->addTab( gl_widget_, "3D View");
    setCentralWidget( tab );

    QStringList args = qApp->arguments(); args.removeFirst();
    OpenImages(args);
    if(!args.isEmpty()) ComputeMatches();
    
    is_video_sequence_ = false;
}

MainWindow::~MainWindow() {
  std::list<Reconstruction *>::iterator iter = reconstructions_.begin();
  for (; iter != reconstructions_.end(); ++iter) {
    (*iter)->ClearCamerasMap();
    (*iter)->ClearStructuresMap(); 
    delete *iter;
  }
  reconstructions_.clear();
  delete gl_widget_;
  gl_widget_ = NULL;
  delete graph_view_;
  graph_view_ = NULL;
  delete grid_layout_;
  grid_layout_ = NULL;
}

void MainWindow::OpenImages() {
    OpenImages(QFileDialog::getOpenFileNames(
                this, tr("Select Images"),"",
                "Pictures (*.png *.jpg *.jpeg *.bmp \
                *.ppm *.pgm *.xpm *.tif *.tiff *.tga);;All Files (*.*)"));
}

void MainWindow::SaveReconstructionFile() {
  QString selected_filter = "";
  QString out_file = QFileDialog::getSaveFileName(
                      this, tr("Export Reconstruction"),"",
                      tr("Blender Script (*.py);;PLY format (*.ply)"),
                      &selected_filter);
  if (out_file.isEmpty()) return;
  
  QFileInfo file_info(out_file);
  QString path_basename = file_info.path();
  path_basename.append("/");
  path_basename.append(file_info.baseName());
  
  uint i = 0;
  std::stringstream s;
  std::list<Reconstruction *>::iterator iter = reconstructions_.begin();
  if (selected_filter == tr("PLY format (*.ply)")) {
    for (; iter != reconstructions_.end(); ++iter) {
      s << path_basename.toStdString() << "-" << i << "."
        << file_info.completeSuffix().toStdString();
      ExportToPLY(**iter, s.str());
      s.str("");
      ++i;
    }
  } else {
    for (; iter != reconstructions_.end(); ++iter) {
      s << path_basename.toStdString() << "-" << i << "."
        << file_info.completeSuffix().toStdString();
      ExportToBlenderScript(**iter, s.str());
      s.str("");
      ++i;
    }
  }
}
void MainWindow::OpenImages( QStringList files ) {
    if (files.isEmpty()) return;
    foreach(ImageView* w,images) {
        grid_layout_->removeWidget(w);
        delete w;
    }
    images.clear();
    int ratio = (int)ceil(sqrt((float)files.count()));
    for (int i = 0; i < files.count(); i++) {
        ImageView* view = new ImageView(i,files[i]);
        connect(view,SIGNAL(enter(int)),SIGNAL(setFilter(int)));
        connect(view,SIGNAL(leave()),SIGNAL(clearFilter()));
        connect(this,SIGNAL(setFilter(int)),view,SLOT(setFilter(int)));
        connect(this,SIGNAL(clearFilter()),view,SLOT(clearFilter()));
        grid_layout_->addWidget(view,i/ratio,i%ratio);
        images << view;
    }
}

bool progressCallback(QProgressDialog &progress, int n) {
    progress.setValue(n);
    return progress.wasCanceled();
}
void MainWindow::WarningNotFunctional() {     
  QMessageBox::warning (this, "Warning", 
                        "This process is STILL in development.");
}

bool MainWindow::SelectDetectorDescriber(eDetector *detector, 
                                         eDescriber *describer) {
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
    *detector = detectorMap[item.toStdString()];
  item = QInputDialog::getItem(this, tr("Choose a describer..."),
                                        tr("Descriptor:"), 
                                        descriptor_list, 3, false, &ok);
  if (ok && !item.isEmpty())    
    *describer = descriptorMap[item.toStdString()];
  
  return ok;
}

void MainWindow::ComputeMatches() {
    QProgressDialog progress("Computing matches...","Abort", 0, 
                             images.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    eDetector detector   = detector::FAST_DETECTOR;
    eDescriber describer = descriptor::DAISY_DESCRIBER;
    SelectDetectorDescriber(&detector, &describer);
    // TODO(julien) create a UI to selection the detector/describer we want
    Detector * pdetector  = detectorFactory(detector);
    Describer* pdescriber = describerFactory(describer);
    nViewMatcher_ = correspondence::nRobustViewMatching(pdetector, pdescriber);

    libmv::vector<std::string> image_vector;
    foreach (ImageView* image, images) 
      image_vector.push_back(image->path().toStdString());
    nViewMatcher_.computeCrossMatch(image_vector);

    matches_.Merge(nViewMatcher_.getMatches());
    UpdateGraph();
    is_video_sequence_ = false;
}

void MainWindow::UpdateGraph() {
  QProgressDialog progress("Updating graph view...","Abort", 0, 
                             images.count(), this);
  progress.setWindowModality(Qt::WindowModal);
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
                matches_.InImage<KeypointFeature>(i);
                features;++features) {
              Matches::TrackID id_track = features.track();
              const KeypointFeature * ref = features.feature();
              const KeypointFeature * f = (KeypointFeature*) 
              //FIXME: could Get() return FeatureT ?
              matches_.Get(j, id_track);
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
  graph_view_->fitInView(graph->sceneRect());
}

ByteImage * ConvertToGrayscale(const ByteImage &imageArrayBytes) {
  ByteImage *arrayGrayBytes = NULL;
  // Grayscale image convertion
  if (imageArrayBytes.Depth() == 3) {
    arrayGrayBytes = new ByteImage ();
    Rgb2Gray<ByteImage, ByteImage>(imageArrayBytes, arrayGrayBytes);
  } else {
    //TODO(julien) Useless: don't copy an already grayscale image
    arrayGrayBytes = new ByteImage (imageArrayBytes);
  }
  return arrayGrayBytes;
}

void MainWindow::ComputeRelativeMatches() {
    QProgressDialog progress("Computing relative matches...","Abort", 0, 
                             images.count(), this);
    WarningNotFunctional();
    progress.setWindowModality(Qt::WindowModal);
    eDetector detector   = detector::FAST_DETECTOR;
    eDescriber describer = descriptor::DAISY_DESCRIBER;
    SelectDetectorDescriber(&detector, &describer);
    // TODO(julien) create a UI to selection the detector/describer we want
    Detector * pdetector  = detectorFactory(detector);
    Describer* pdescriber = describerFactory(describer);
    correspondence::ArrayMatcher_Kdtree<float> *matcher = 
      new correspondence::ArrayMatcher_Kdtree<float>();

    // Track the sequence of images
    std::list<std::string> image_list;
    foreach (ImageView* image, images) 
      image_list.push_back(image->path().toStdString());
    
    libmv::tracker::FeaturesGraph all_features_graph;
    libmv::tracker::FeaturesGraph prev_features_graph;
    tracker::RobustTracker points_tracker(pdetector, pdescriber, matcher);
    size_t image_index = 0;
    std::list<std::string>::iterator image_list_iterator = image_list.begin();
    for (; image_list_iterator != image_list.end(); ++image_list_iterator) {
      std::string image_path = (*image_list_iterator);

      ByteImage imageArrayBytes;
      ReadImage (image_path.c_str(), &imageArrayBytes);
      ByteImage *arrayGrayBytes = ConvertToGrayscale(imageArrayBytes);

      Image image(arrayGrayBytes);
      libmv::tracker::FeaturesGraph new_features_graph;
      libmv::Matches::ImageID new_image_id = 0;
      points_tracker.Track(image,
                           prev_features_graph,
                           &new_features_graph,
                           &new_image_id,
                           true);
      prev_features_graph.Clear();
      prev_features_graph.matches_.Merge(new_features_graph.matches_);
      all_features_graph.Merge(new_features_graph);
      image_index++;
    }
    
    matches_.Merge(all_features_graph.matches_);
    UpdateGraph();
    is_video_sequence_ = true;
    ExportMatchesToTxt(all_features_graph.matches_, "./matches.txt");
    // TODO(julien) Delete the features graph
    //all_features_graph.DeleteAndClear();
}

void MainWindow::LoadMatches() {
  QString s = QFileDialog::getOpenFileName(
                     this, tr("Select Matches"),"",
                     "Text files (*.txt);;All Files (*.*)");
  if (s.isEmpty()) return;
 
  // Imports matches
  FeatureSet *fs = new FeatureSet();
  ImportMatchesFromTxt(s.toStdString(), &matches_, fs);
  UpdateGraph();
  is_video_sequence_ = true;
  // TODO(julien) Delete the features graph
  // delete fs
}

void MainWindow::ComputeUncalibratedReconstruction() {
  QProgressDialog progress("Computing uncalibrated reconstruction...","Abort",0,
                           images.count(), this);
  progress.setWindowModality(Qt::WindowModal);
  Vec2u image_size;
  PinholeCamera * camera = NULL;
  
  if (images.count()<2)
    return;
  // Estimation of the second image
 
  progress.setLabelText("Selecting best initial images...");
  std::list<libmv::vector<Matches::ImageID> > connected_graph_list;
  SelectEfficientImageOrder(matches_,  &connected_graph_list);
  
  // TODO(julien) put the following in the reconstruction lib. 
  Matches matches_inliers;
  size_t image_id = 0;
  size_t index_image_graph = 0;
  Reconstruction *recons = NULL;
  std::list<libmv::vector<Matches::ImageID> >::iterator graph_iter =
    connected_graph_list.begin();
  for (; graph_iter != connected_graph_list.end(); ++graph_iter) {
    if (graph_iter->size() >= 2) {
      recons = new Reconstruction();
      reconstructions_.push_back(recons);
      progress.setLabelText("Initial Motion Estimation");
      VLOG(1) << " -- Initial Motion Estimation --  " << std::endl;
      ReconstructFromTwoUncalibratedViews(matches_, 
                                          (*graph_iter)[0], 
                                          (*graph_iter)[1], 
                                          &matches_inliers, 
                                          recons);
      index_image_graph = 0;
      image_id = (*graph_iter)[index_image_graph];
      camera = dynamic_cast<PinholeCamera*>(
        recons->GetCamera(image_id));
      if (camera) {
        image_size << images[image_id]->GetImageWidth(), 
                      images[image_id]->GetImageHeight();
        camera->set_image_size(image_size); 
      }
      index_image_graph = 1;
      image_id = (*graph_iter)[index_image_graph];
      camera = dynamic_cast<PinholeCamera*>(
        recons->GetCamera(image_id));
      if (camera) {
        image_size << images[image_id]->GetImageWidth(), 
                      images[image_id]->GetImageHeight();
        camera->set_image_size(image_size); 
      }
      progressCallback(progress, 1);
      
      progress.setLabelText("Initial Intersection");
      VLOG(1) << " -- Initial Intersection --  " << std::endl;
      size_t minimum_num_views = 2;
      PointStructureTriangulationUncalibrated(matches_inliers, 
                                              image_id,
                                              minimum_num_views, 
                                              recons);
      // Performs projective bundle adjustment
      //VLOG(1) << " -- Projective Bundle Adjustment --  " << std::endl;
      
      // Estimation of the pose of other images by resection
      minimum_num_views = 3;
      for (index_image_graph = 2; index_image_graph < graph_iter->size();
          ++index_image_graph) {
        image_id = (*graph_iter)[index_image_graph];
        progress.setLabelText("Incremental Resection");
        VLOG(1) << " -- Incremental Resection --  " << std::endl;
        UncalibratedCameraResection(matches_, image_id,
                                    &matches_inliers, recons);     
        
        camera = dynamic_cast<PinholeCamera*>(
          recons->GetCamera(image_id));
        if (camera) {
          image_size << images[image_id]->GetImageWidth(),
                        images[image_id]->GetImageHeight();
          camera->set_image_size(image_size); 
        }
        // TODO(julien) Avoid to retriangulate, prefer projective BA
        progress.setLabelText("Retriangulation");
        VLOG(1) << " -- Retriangulation --  " << std::endl;
        PointStructureRetriangulationUncalibrated(matches_inliers, 
                                                  image_id,
                                                  recons);
        
        progress.setLabelText("Incremental Intersection");
        VLOG(1) << " -- Incremental Intersection --  " << std::endl;
        // TODO(julien) this do nothing (no points)...fix it
        PointStructureTriangulationUncalibrated(matches_inliers, 
                                                image_id,
                                                minimum_num_views, 
                                                recons);

        // TODO(julien) Performs projective bundle adjustment
        progressCallback(progress, index_image_graph);
      }
      DrawAllStructures(*recons);
    }
  }
  //matches_.Clear();
  //matches_.Merge(matches_inliers);
  UpdateGraph();
}

void MainWindow::ComputeCalibratedReconstruction() {
  QProgressDialog progress("Computing calibrated reconstruction...","Abort", 0,
                           images.count(), this);
  progress.setWindowModality(Qt::WindowModal);
  Vec2u image_size;
  
  if (images.count()<2)
    return;
  
  double focal = 1639.47;
  bool ok;
    
  //TODO(julien) create a better UI...
  focal = QInputDialog::getDouble(this, 
          tr("Set the focal length (in pixels)"),
          tr("Focal (px) = image width (px) * focal length (mm) / CCD width (mm):"), focal, 0, 10000, 4, &ok);
  if (!ok) return;
  /*double cu = images[0]->GetImageWidth()/2 - 0.5, 
         cv = images[0]->GetImageHeight()/2 - 0.5;
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
      0,   0,   1;*/
      
  if (is_video_sequence_) {
    progress.setLabelText("Euclidean Reconstruction From Video");
    VLOG(1) << "Euclidean Reconstruction From Video..." << std::endl;
    EuclideanReconstructionFromVideo(matches_, 
                                     images[0]->GetImageWidth(), 
                                     images[0]->GetImageHeight(),
                                     focal,
                                     &reconstructions_);
    VLOG(1) << "Euclidean Reconstruction From Video...[DONE]" << std::endl;
    
    //progress.setLabelText("Updating 2D/3D views");
    DrawAllStructures(**reconstructions_.begin());
    
    progress.setLabelText("Exporting reconstructions");
    int i = 0;
    std::list<Reconstruction *>::iterator iter = reconstructions_.begin();
    for (; iter != reconstructions_.end(); ++iter) {
      std::stringstream s;
      if (reconstructions_.size() > 1)
        s << "./out" << "-" << i << ".py";
      else
        s << "./out.py";
      ExportToBlenderScript(**iter, s.str());
    }
    // Cleaning
    /*progress.setLabelText("Cleaning reconstructions");
    VLOG(2) << "Cleaning." << std::endl;
    iter = reconstructions_.begin();
    for (; iter != reconstructions_.end(); ++iter) {
      (*iter)->ClearCamerasMap();
      (*iter)->ClearStructuresMap(); 
      delete *iter;
    }
    reconstructions_.clear();*/
  } else {
    WarningNotFunctional();
  }
  //matches_.Clear();
  //matches_.Merge(matches_inliers);
  UpdateGraph();
}

void MainWindow::ComputeMetricRectification() {
  WarningNotFunctional();
  QProgressDialog progress("Computing reconstruction...","Abort", 0,
                           images.count(),this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setLabelText("Metric rectification");
  // Metric rectification
  std::list<Reconstruction *>::iterator iter = reconstructions_.begin();
  for (; iter != reconstructions_.end(); ++iter)
    UpgradeToMetric(matches_, *iter);
}

void MainWindow::ComputeBA() {
  QProgressDialog progress("Computing reconstruction...","Abort", 0,
                           images.count(),this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setLabelText("Bundle adjustment");
  // Performs bundle adjustment
  std::list<Reconstruction *>::iterator iter = reconstructions_.begin();
  for (; iter != reconstructions_.end(); ++iter)
    MetricBundleAdjust(matches_, *iter);
  progress.setLabelText("Reconstruction done.");
}

void MainWindow::DrawAllStructures(const Reconstruction &recons) {
  PointStructure *ps = NULL;
  libmv::vector<Vec3> struct_coords;
  struct_coords.reserve(recons.GetNumberStructures());
  std::map<StructureID, Structure *>::const_iterator it = 
   recons.structures().begin();
  for (; it != recons.structures().end(); ++it) {
    ps = dynamic_cast<PointStructure *>(it->second);
    if (ps) {
      struct_coords.push_back(ps->coords_affine());
    }
  }
  gl_widget_->AddNewStructure(struct_coords);
}

void MainWindow::DrawNewStructures(const libmv::vector<StructureID> &struct_ids,
                                   const Reconstruction &recons) {
  PointStructure *ps = NULL;
  libmv::vector<Vec3> struct_coords;
  struct_coords.reserve(struct_ids.size());
  for (size_t s = 0; s < struct_ids.size(); ++s) {
    ps = dynamic_cast<PointStructure *>(
     recons.GetStructure(struct_ids[s]));
    if (ps) {
      struct_coords.push_back(ps->coords_affine());
    }
  }
  gl_widget_->AddNewStructure(struct_coords);
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

