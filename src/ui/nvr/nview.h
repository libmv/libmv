#pragma once
#include <list>
#include <string>

#include <QList>
#include <QImage>
#include <QDebug>
#include <QApplication>
#include <QMainWindow>
#include <QProgressDialog>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVector2D>
#include <QWheelEvent>

#include "libmv/tools/tool.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/nRobustViewMatching.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "libmv/reconstruction/reconstruction.h"
#include "libmv/descriptor/descriptor_factory.h"
#include "libmv/detector/detector_factory.h"

#include "glwidget.h"

// TODO(julien) avoid using namespace in headers!
using namespace libmv;
using namespace libmv::correspondence;
using namespace libmv::descriptor;
using namespace libmv::detector;

/// an image displayed with matches
class ImageView : public QWidget {
    Q_OBJECT
public:
    ImageView( int i, QString path ) : index(i), _path(path),  image(QPixmap(path)), filter(-1) {}
    QString path() { return _path; }
    void setFeatures(QVector<QVector<KeypointFeature> >);
    int GetImageWidth()  { return image.width();  }
    int GetImageHeight() { return image.height(); }
public slots:
    void setFilter(int i);
    void clearFilter();
signals:
    void enter(int i);
    void leave();
protected:
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
public: //private:
    int index;
    QString _path;
    QPixmap image;
    QVector<QVector<KeypointFeature> > features;
    int filter;
};

struct Node;

// TODO(julien) put this (Edge, node, graph, GraphView) in a widget (.h/.cc)
/// a line linking two nodes
struct Edge : public QGraphicsLineItem {
  Edge(Node* a, Node* b) : a(a), b(b) { setZValue(-1); }
  Node* a;
  Node* b;
};

/// an image in the match graph
struct Node : public QGraphicsPixmapItem {
//Springs
  QMap<Node*,Edge*> edges;

//RK4 integration
  QVector2D position[4];
  QVector2D velocity[4];
  QVector2D acceleration[4];
};

/// a graph of images with edge between matching nodes
class Graph : public QGraphicsScene {
  Q_OBJECT
public:
  Graph();
protected:
  void timerEvent(QTimerEvent *event);
  //void itemMoved();
public: //private:
  void evaluate( float dt, int current );
  QList<Node*> nodes;
  QList<Edge*> edges;
};

class GraphView : public QGraphicsView {
    Q_OBJECT
protected:
    void wheelEvent(QWheelEvent* e) { scale(1.0+e->delta()/360.0,
                                            1.0+e->delta()/360.0); }
};

/// QMainWindow with Match/3D View and actions/graph QDockWidgets
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openImages();
    void openImages( QStringList filenames );
    void SaveReconstructionFile();
    void computeMatches();
    void computeRelativeMatches();
    void computeUncalibratedReconstruction();
    void computeCalibratedReconstruction();
    void computeMetricRectification();
    void computeBA();
    
    void UpdateGraph();
    void warningNotFunctional();
    bool SelectDetectorDescriber(eDetector *detector, eDescriber *describer);
signals:
    void setFilter(int i);
    void clearFilter();
private:
    void DrawAllStructures(const Reconstruction &recons);
    void DrawNewStructures(const libmv::vector<StructureID> &struct_ids,
                           const Reconstruction &recons);
    nRobustViewMatching nViewMatcher_;
    QList<ImageView*> images;
    Graph* graph;
    std::list<Reconstruction *> reconstructions_;
    Matches matches_;
    bool is_video_sequence_;

    QGridLayout *grid_layout_;
    GraphView   *graph_view_;
    GLWidget    *gl_widget_;
};
