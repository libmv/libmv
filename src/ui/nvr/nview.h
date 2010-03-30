#pragma once

#include <QList>
#include <QImage>
#include <QDebug>
#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QPainter>

#include "libmv/tools/tool.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/feature_matching_FLANN.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/star_detector.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/daisy_descriptor.h"
#include "libmv/descriptor/simpliest_descriptor.h"
#include "libmv/image/array_nd.h"
#include "libmv/image/image.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/bundle.h"

using namespace libmv;

class nMatch;
class nImage;

/// Matches between two images
class nMatch {
public:
    nMatch( nImage* A, nImage* B );
    Matches::Points features( nImage* image );
private:
    libmv::Matches mvMatch;
    nImage* A;
    nImage* B;
};

/// QImage with features
class nImage : public QObject, public QImage {
    Q_OBJECT
public:
    nImage( QString path ) : QImage(path) {}
    void update();
public slots:
    void computeFeatures();
    bool hasMatch( nImage* other );
    QMap<nImage*,nMatch*> allMatches();
    void clearMatches();
signals:
    void updated();
public:
    FeatureSet featureSet;
    QMap<nImage*,nMatch*> matches;
    /*double focalDistance;
    libmv::Mat3 K;
    libmv::Mat3 R;
    libmv::Vec3 t;*/
};

/// QWidget displaying an nImage
class MatchView : public QWidget {
    Q_OBJECT
public slots:
    void setImage( nImage* );
protected:
    void paintEvent(QPaintEvent *);
private:
    nImage* image;
};

/// QMainWindow with Match/3D View and actions/graph QDockWidgets
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
public slots:
    void addImages();
    void addImages( QStringList filenames );
    void saveBlender();
    void computeMatches();
    void focalFromFundamental();
    void metricReconstruction();
    void metricBundle();
private:
    QList<nImage*> images;
    libmv::Mat3 F;
    vector<libmv::Vec3> X;
    vector<libmv::Vec3f> X_colors;

    QGridLayout* gridLayout;
    //QGLView* view3D
};
