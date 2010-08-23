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
#include <string>

#include "libmv/tools/tool.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/nRobustViewMatching.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"

using namespace libmv;

/// Image object : (a QImage and his disk path)
class nImage : public QObject, public QImage {
    Q_OBJECT
public:
    nImage( QString path ) : QImage(path) {
      _path = path.toStdString();
    }
    void update();
signals:
    void updated();
public:
    std::string _path;
    /*double focalDistance;
    libmv::Mat3 K;
    libmv::Mat3 R;
    libmv::Vec3 t;*/
};

/// QWidget displaying a nImage
class MatchView : public QWidget {
    Q_OBJECT
public slots:
    void setImage( nImage*, const libmv::Matches * matches, int index );
protected:
    void paintEvent(QPaintEvent *);
private:
    nImage* image;
    const libmv::Matches * _mMatches; // SceneMatches
    int _indexInMatchesTable;         // Reference image number in the scene
};

/// QMainWindow with Match/3D View and actions/graph QDockWidgets
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
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
    libmv::correspondence::nRobustViewMatching nViewMatcher;

    //To handle the point cloud
    //vector<libmv::Vec3> X;
    //vector<libmv::Vec3f> X_colors;

    QGridLayout* gridLayout;
    //QGLView* view3D
};
