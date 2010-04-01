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
    {QPushButton* button = new QPushButton("Add Images...");
        connect(button,SIGNAL(clicked()),SLOT(addImages()));
        layout->addWidget(button);
    }
    {QPushButton* button = new QPushButton("Compute Matches...");
        connect(button,SIGNAL(clicked()),SLOT(computeMatches()));
        layout->addWidget(button);
    }
    addDockWidget(Qt::LeftDockWidgetArea,actionsDock);
    addDockWidget(Qt::LeftDockWidgetArea,new QDockWidget("Match Graph",this)); //dummy

    QTabWidget* tab = new QTabWidget(this);
    QWidget* gridView = new QWidget(tab);
    gridLayout = new QGridLayout(gridView);
    gridView->setLayout(gridLayout);
    tab->addTab( gridView, "Match View");
    tab->addTab( new QWidget(), "3D View"); //dummy
    setCentralWidget( tab );

    addImages(QStringList()
              <<"/home/matt/Documents/a.jpg"
              <<"/home/matt/Documents/b.jpg"
              <<"/home/matt/Documents/c.jpg"
              <<"/home/matt/Documents/d.jpg");
}

void MainWindow::addImages() { addImages(QFileDialog::getOpenFileNames(this,"Select Images","","Pictures (*.jpg); All Files")); }
void MainWindow::addImages( QStringList filenames ) {
    foreach( QString filename, filenames ) {
        qDebug() << filename << QFile(filename).exists();
        if(QFile(filename).exists()) images << new nImage(filename);
    }
    int ratio = (int)ceil(sqrt((float)images.count()));
    for(int i=0;i<images.count();i++) {
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
    p.setTransform(QTransform::fromScale((float)width()/image->width(),(float)width()/image->width()));
    p.setBrush(Qt::NoBrush);
    foreach( nMatch* match, image->allMatches().values() ) {
        for(Matches::Points r = match->features(image); r; ++r ) {
            const PointFeature* f = r.feature();
            p.setPen(QColor(255,255,255));
            p.drawEllipse(QPoint(f->x(), f->y()), 4, 4 );
            /*glPushMatrix();
            glTranslatef(r.feature()->x(), r.feature()->y(), 0.0f);
            // Convert from gaussian scale to pixel scale (see surf.h).
            float scale = r.feature()->scale;
            glScalef(scale, scale, scale);
            // TODO(pau) when orientation will be detected, ensure that this is
            //           turning in the right sense and the right units (deg vs rad).
            glRotatef(r.feature()->orientation*180.0f/3.14159f, 0.0f, 0.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex2f(-1, -1); glVertex2f( 1, -1); // Square Box.
            glVertex2f( 1, -1); glVertex2f( 1,  1);
            glVertex2f( 1,  1); glVertex2f(-1,  1);
            glVertex2f(-1,  1); glVertex2f(-1, -1);        glVertex2f(0,  0); glVertex2f(0, -1);   // Vertical line to see orientation.
            glEnd();
            glPopMatrix();*/
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
    scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 30, true));

    vector<Feature *> features;
    Image im(new Array3Du(image));
    detector->Detect(im, &features, NULL);

    vector<descriptor::Descriptor *> descriptors;
    scoped_ptr<descriptor::Describer> describer(descriptor::CreateSimpliestDescriber());
    describer->Describe(features, im, NULL, &descriptors);

    // Copy data.
    featureSet.features.resize(descriptors.size());
    for(int i = 0;i < descriptors.size(); i++) {
        KeypointFeature& feature = featureSet.features[i];
        feature.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
        *(PointFeature*)(&feature) = *(PointFeature*)features[i];
    }

    DeleteElements(&descriptors);
    DeleteElements(&features);
}
bool nImage::hasMatch(nImage *other) { return matches.contains(other); }
QMap<nImage*,nMatch*> nImage::allMatches() { return matches; }
void nImage::clearMatches() { matches.clear(); }
void nImage::update() { emit updated(); }

nMatch::nMatch(nImage *A, nImage *B) : A(A), B(B) {
    FindCandidateMatches( A->featureSet, B->featureSet, &mvMatch );
    A->matches[B]=this; B->matches[A]=this;
}
Matches::Points nMatch::features( nImage* image ) { return mvMatch.InImage<PointFeature>(image==A?0:1); }

void MainWindow::computeMatches() {
    int i=0; foreach( nImage* image, images ) {
        statusBar()->showMessage(QString("Computing features (%1/%2)").arg(i++).arg(images.count()));
        image->computeFeatures();
        QApplication::processEvents();
    }
    int a=0; foreach( nImage* A, images ) {
        A->clearMatches();
        int b=0; foreach( nImage* B, images ) {
            if( A != B && !A->hasMatch(B) ) {
                statusBar()->showMessage(QString("Computing matches (%1 <-> %2)").arg(a).arg(b++));
                new nMatch( A, B );
                A->update(); B->update();
            }
            QApplication::processEvents();
        }
        a++;
    }
    statusBar()->showMessage("Done.");
    /*libmv::Matches robustMatches;
    // Construct matrices containing the matches.
    vector<Mat> x;
    vector<int> tracks;
    vector<int> imageIndices;
    for(int i=0;i<images.count();i++) images.push_back(imageIndices);
    PointMatchMatrices(matches, imageIndices, &tracks, &x);

    // Compute Fundamental matrix and inliers.
    vector<int> inliers;
    FundamentalFromCorrespondences7PointRobust(x[0], x[1], 1, F, &inliers);
    // Build new correspondence graph containing only inliers.
    for (int j = 0; j < inliers.size(); ++j) {
        int k = inliers[j];
        for (int i = 0; i < 2; ++i) {
            robustMatches->Insert(images[i], tracks[k], matches.Get(images[i], tracks[k]));
        }
    }

    // Compute Fundamental matrix using all inliers.
    TwoViewPointMatchMatrices(*robustMatches, 0, 1, &x);
    vector<Mat3> Fs;
    fundamental::kernel::NormalizedEightPointKernel::Solve(x[0], x[1], &Fs);
    *F = Fs[0];
    NormalizeFundamental(*F, F);
    std::swap( matches, robustMatches );*/
}

void MainWindow::focalFromFundamental() {
    /*if (document.matches.NumTracks() == 0)  {
        QMessageBox::information(this, tr("TVR"),
                                 tr("Cannot compute Focal from Fundamental.\nFundamental was not computed."));
        return;
    }

    QMainWindow::statusBar()->showMessage("Start : FocalFromFundamental");

    vector<Mat> xs;
    TwoViewPointMatchMatrices(document.matches, 0, 1, &xs);
    Mat &x1 = xs[0];
    Mat &x2 = xs[1];

    libmv::Vec2 p0((document.images[0].width() - 1) / 2.,
                   (document.images[0].height() - 1) / 2.);
    libmv::Vec2 p1((document.images[1].width() - 1) / 2.,
                   (document.images[1].height() - 1) / 2.);

    bool use_hartleys_method = false;
    if (use_hartleys_method) {
        libmv::FocalFromFundamental(document.F, p0, p1,
                                    &document.focal_distance[0],
                                    &document.focal_distance[1]);
        LOG(INFO) << "focal 0: " << document.focal_distance[0]
                << " focal 1: " << document.focal_distance[1] << "\n";
    } else {
        double fmin_mm = 10;
        double fmax_mm = 150;
        double sensor_width_mm = 36; // Assuming a full-sized sensor 1x equiv.
        double width_pix = document.images[0].width();
        libmv::FocalFromFundamentalExhaustive(document.F, p0, x1, x2,
                                              fmin_mm / sensor_width_mm * width_pix,
                                              fmax_mm / sensor_width_mm * width_pix,
                                              100,
                                              &document.focal_distance[0]);
        document.focal_distance[1] = document.focal_distance[0];

        LOG(INFO) << "focal: " << document.focal_distance[0]
                << "pix    35mm equiv: " << document.focal_distance[0]
                * sensor_width_mm / width_pix
                << "\n";
    }

    UpdateViewers();

    QMainWindow::statusBar()->showMessage(QString("End : FocalFromFundamental. ")
                                          + "Focal (pix) estimated to : " + QString::number(document.focal_distance[0])
                                          + "for Image 0 and to : " + QString::number(document.focal_distance[1])
                                          + "for Image 1." );*/
}

void MainWindow::metricReconstruction() {
    /*if (document.matches.NumTracks() == 0)  {
        QMessageBox::information(this, tr("TVR"),
                                 tr("Cannot compute Metric reconstruction."\
                                    "\nGeometric correspondence list is empty."));
        return;
    }

    QMainWindow::statusBar()->showMessage("Start : MetricReconstruction");
    using namespace libmv;

    Vec2 p0((document.images[0].width() - 1) / 2.,
            (document.images[0].height() - 1) / 2.);
    Vec2 p1((document.images[1].width() - 1) / 2.,
            (document.images[1].height() - 1) / 2.);
    double f0 = document.focal_distance[0];
    double f1 = document.focal_distance[1];
    Mat3 K0, K1;
    K0 << f0,  0, p0(0),
    0, f0, p0(1),
    0,  0,     1;
    K1 << f1,  0, p1(0),
    0, f1, p1(1),
    0,  0,     1;

    // Compute essential matrix
    Mat3 E;
    EssentialFromFundamental(document.F, K0, K1, &E);

    // Get matches from the correspondence structure.
    vector<Mat> xs(2);
    TwoViewPointMatchMatrices(document.matches, 0, 1, &xs);
    Mat &x0 = xs[0];
    Mat &x1 = xs[1];

    // Recover R, t from E and K
    Mat3 R;
    Vec3 t;
    MotionFromEssentialAndCorrespondence(E, K0, x0.col(0), K1, x1.col(0), &R, &t);

    LOG(INFO) << "R:\n" << R << "\nt:\n" << t;

    document.K[0] = K0;
    document.R[0] = Mat3::Identity();
    document.t[0] = Vec3::Zero();
    document.K[1] = K1;
    document.R[1] = R;
    document.t[1] = t;

    // Triangulate features.
    vector<Mat34> Ps(2);
    P_From_KRt(document.K[0], document.R[0], document.t[0], &Ps[0]);
    P_From_KRt(document.K[1], document.R[1], document.t[1], &Ps[1]);

    int n = x0.cols();
    document.X.resize(n);
    document.X_colors.resize(n);
    for (int i = 0; i < n; ++i) {
        Mat2X x(2, 2);
        x.col(0) = x0.col(i);
        x.col(1) = x1.col(i);
        Vec4 X;
        NViewTriangulate(x, Ps, &X);
        document.X[i] = libmv::HomogeneousToEuclidean(X);

        // Get 3D point color from first image.
        QRgb rgb = document.images[0].pixel(int(round(x0(0,i))),
                                            int(round(x0(1,i))));
        document.X_colors[i] << qBlue(rgb), qGreen(rgb), qRed(rgb);
        document.X_colors[i] /= 255;
    }
    QMainWindow::statusBar()->showMessage("End : MetricReconstruction");*/
}

void MainWindow::metricBundle() {
    /*if (document.matches.NumTracks() == 0)  {
        QMessageBox::information(this, tr("TVR"),
                                 tr("Cannot compute Metric bundle adjustement."\
                                    "\nGeometric correspondence list is empty."));
        return;
    }

    QMainWindow::statusBar()->showMessage("Start : MetricBundle");
    using namespace libmv;

    std::vector<Mat3> K(2);
    std::vector<Mat3> R(2);
    std::vector<Vec3> t(2);
    for (int i = 0; i < 2; ++i) {
        K[i] = document.K[i];
        R[i] = document.R[i];
        t[i] = document.t[i];
    }
    std::vector<Mat2X> x(2);
    vector<Mat> xs(2);
    TwoViewPointMatchMatrices(document.matches, 0, 1, &xs);
    for (int i = 0; i < 2; ++i) {
        x[i] = xs[i];
    }

    Mat3X X(3, document.X.size());
    for (int i = 0; i < X.cols(); ++i) {
        X.col(i) = document.X[i];
    }
    assert(x[0].cols() == X.cols());
    assert(x[1].cols() == X.cols());

    EuclideanBAFull(x, &K, &R, &t, &X);                 // Run BA.

    for (int i = 0; i < 2; ++i) {
        document.K[i] = K[i];
        document.R[i] = R[i];
        document.t[i] = t[i];
    }
    for (int i = 0; i < X.cols(); ++i) {
        document.X[i] = X.col(i);
    }
    //TODO(pmoulon) : Display residuals before and after bundle adjustment)
    QMainWindow::statusBar()->showMessage("End : MetricBundle");*/
}

/*void MatchViewer::DrawCandidateMatches() {
  float xoff[2] = { screen_images_[0].posx, screen_images_[1].posx};
  float yoff[2] = { screen_images_[0].posy, screen_images_[1].posy};

  int nb = 0;
  glBegin(GL_LINES);  // TODO(keir): Note that this will break with > 2 images.
  for (Matches::Points r =
      document_->matches.AllReversed<PointFeature>(); r; ++r, ++nb) {

    //Compute a color that is dependent of the point position
    float colorFactor = sqrt( nb + abs(r.feature()->x()) + abs(r.feature()->y()) );
    float red = 1000.f * colorFactor,
          g = 10000.f * colorFactor,
          b = 100000.f * colorFactor;
    glColor3f( red -= (int)red, g -= (int)g, b -= (int)b);

    glVertex2f(xoff[r.image()] +  r.feature()->x(),
               yoff[r.image()] +  r.feature()->y());
  }
  glEnd();
  //Restore white color as default color
  glColor3f( 1.0f, 1.0f, 1.0f);
}*/


void MainWindow::saveBlender() {
    /*QString filename = QFileDialog::getSaveFileName(this,
                                                        "Save as Blender Script", "", "Blender Python Script (*.py)");
        if (filename.isNull())
            return;
        document.SaveAsBlender(filename.toAscii().data());

        // Display information to the user.
        QMainWindow::statusBar()->showMessage(
                "Scenes correctly exported as Blender script file : " + filename);*/
}

/*void MainWindow::CreateActions() {
    open_images_action_ = new QAction(tr("&Open Images..."), this);
    open_images_action_->setShortcut(tr("Ctrl+O"));
    open_images_action_->setStatusTip(tr("Open an image pair"));
    connect(open_images_action_, SIGNAL(triggered()),
            this, SLOT(OpenImages()));

    view_2d_action_ = new QAction(tr("&2D View"), this);
    view_2d_action_->setShortcut(tr("2"));
    view_2d_action_->setStatusTip(tr("Show a 2D view of the matches"));
    connect(view_2d_action_, SIGNAL(triggered()),
            this, SLOT(Show2DView()));

    view_3d_action_ = new QAction(tr("&3D View"), this);
    view_3d_action_->setShortcut(tr("3"));
    view_3d_action_->setStatusTip(tr("Show a 3D view of the reconstruction"));
    connect(view_3d_action_, SIGNAL(triggered()),
            this, SLOT(Show3DView()));

    save_blender_action_ = new QAction(tr("&Save as Blender..."), this);
    save_blender_action_->setShortcut(tr("Ctrl+S"));
    save_blender_action_->setStatusTip(tr("Save Scene as a Blender Script"));
    connect(save_blender_action_, SIGNAL(triggered()),
            this, SLOT(SaveBlender()));

    compute_features_action_ = new QAction(tr("&Compute Features"), this);
    compute_features_action_->setStatusTip(tr("Compute Features"));
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

    metric_reconstruction_action_ = new QAction(tr("&Metric Reconstruction"),
                                                this);
    metric_reconstruction_action_->setStatusTip(tr("Compute a metric "
                                                   "reconstrution given the current focal length estimates"));
    connect(metric_reconstruction_action_, SIGNAL(triggered()),
            this, SLOT(MetricReconstruction()));

    metric_bundle_action_ = new QAction(tr("&Bundle Adjust"),
                                        this);
    metric_bundle_action_->setStatusTip(tr("Perform non-linear "
                                           "optimization of camera parameters and points."));
    connect(metric_bundle_action_, SIGNAL(triggered()),
            this, SLOT(MetricBundle()));
}*/
