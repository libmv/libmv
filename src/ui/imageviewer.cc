#include <QColor>
#include <QtGui>
#include <stdio.h>
#include <iostream>
using namespace std;

#include "libmv/numeric/numeric.h"
#include "libmv/image/image.h"
#include "libmv/image/convolve.h"
#include "third_party/flens/flens.h"
#include "ui/imageviewer.h"
#include "ui/scrubber.h"

using libmv::_;

ImageViewer::ImageViewer()
{
  imageLabel = new QLabel;
  imageLabel->setBackgroundRole(QPalette::Base);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);

  scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(imageLabel);

  setCentralWidget(scrollArea);

  Scrubber *scrubber = new Scrubber;
  scrubber->setNumItems(20);
  scrubber->setItem(0, 1);
  scrubber->setItem(4, 1);
  scrubber->setItem(5, 1);
  scrubber->setItem(18, 1);
  statusBar()->addPermanentWidget(scrubber, 1);

  createActions();
  createMenus();

  setWindowTitle(tr("Image Viewer"));
  resize(500, 400);
}

void ImageViewer::open()
{
  QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty()) {

    libmv::ByteImage mv_image;
    assert(libmv::ReadPgm(fileName.toStdString().c_str(), &mv_image));
    libmv::FloatImage float_mv_image;
    float_mv_image.CopyFrom(mv_image);
    libmv::FloatImage blurred_mv_image;
    libmv::Vec gauss, dgauss;
    libmv::ComputeGaussianKernel(5, &gauss, &dgauss);
    libmv::ConvolveHorizontal(float_mv_image,
                              gauss,
                              &blurred_mv_image);
//    libmv::ConvolveVertical(blurred_mv_image,
//                              gauss,
//                              &float_mv_image);
//    libmv::ConvolveVertical(float_mv_image,
//                              gauss,
//                              &blurred_mv_image);
//    mv_image.CopyFrom(blurred_mv_image);
    libmv::ConvertFloatImageToByteImage(blurred_mv_image, &mv_image);
//    mv_image.CopyFrom(float_mv_image);

    printf("w=%d,h=%d\n", mv_image.Width(), mv_image.Height());

    QImage image(mv_image.Data(), mv_image.Width(), mv_image.Height(),
                 QImage::Format_Indexed8);
    image.setNumColors(256);
    for (int i = 0, count = image.numColors(); i < count; ++i) {
      image.setColor(i, qRgb(i,i,i));
    }

    if (image.isNull()) {
      QMessageBox::information(this, tr("Image Viewer"),
          tr("Cannot load %1.").arg(fileName));
      return;
    }
    QPixmap qp = QPixmap::fromImage(image);
    imageLabel->setPixmap(qp);

    scaleFactor = 1.0;

    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
      imageLabel->adjustSize();
  }
}

void ImageViewer::print()
{
  Q_ASSERT(imageLabel->pixmap());
  QPrintDialog dialog(&printer, this);
  if (dialog.exec()) {
    QPainter painter(&printer);
    QRect rect = painter.viewport();
    QSize size = imageLabel->pixmap()->size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(imageLabel->pixmap()->rect());
    painter.drawPixmap(0, 0, *imageLabel->pixmap());
  }
}

void ImageViewer::zoomIn()
{
  scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
  scaleImage(0.8);
}

void ImageViewer::normalSize()
{
  imageLabel->adjustSize();
  scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
  bool fitToWindow = fitToWindowAct->isChecked();
  scrollArea->setWidgetResizable(fitToWindow);
  if (!fitToWindow) {
    normalSize();
  }
  updateActions();
}


void ImageViewer::about()
{
  QMessageBox::about(this, tr("About Image Viewer"),
      tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
        "and QScrollArea to display an image. QLabel is typically used "
        "for displaying a text, but it can also display an image. "
        "QScrollArea provides a scrolling view around another widget. "
        "If the child widget exceeds the size of the frame, QScrollArea "
        "automatically provides scroll bars. </p><p>The example "
        "demonstrates how QLabel's ability to scale its contents "
        "(QLabel::scaledContents), and QScrollArea's ability to "
        "automatically resize its contents "
        "(QScrollArea::widgetResizable), can be used to implement "
        "zooming and scaling features. </p><p>In addition the example "
        "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  printAct = new QAction(tr("&Print..."), this);
  printAct->setShortcut(tr("Ctrl+P"));
  printAct->setEnabled(false);
  connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
  zoomInAct->setShortcut(tr("Ctrl++"));
  zoomInAct->setEnabled(false);
  connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

  zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
  zoomOutAct->setShortcut(tr("Ctrl+-"));
  zoomOutAct->setEnabled(false);
  connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

  normalSizeAct = new QAction(tr("&Normal Size"), this);
  normalSizeAct->setShortcut(tr("Ctrl+S"));
  normalSizeAct->setEnabled(false);
  connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

  fitToWindowAct = new QAction(tr("&Fit to Window"), this);
  fitToWindowAct->setEnabled(false);
  fitToWindowAct->setCheckable(true);
  fitToWindowAct->setShortcut(tr("Ctrl+F"));
  connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  aboutAct = new QAction(tr("&About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ImageViewer::createMenus()
{
  fileMenu = new QMenu(tr("&File"), this);
  fileMenu->addAction(openAct);
  fileMenu->addAction(printAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = new QMenu(tr("&View"), this);
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addAction(normalSizeAct);
  viewMenu->addSeparator();
  viewMenu->addAction(fitToWindowAct);

  helpMenu = new QMenu(tr("&Help"), this);
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(viewMenu);
  menuBar()->addMenu(helpMenu);
}

void ImageViewer::updateActions()
{
  zoomInAct->setEnabled(!fitToWindowAct->isChecked());
  zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
  normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
  Q_ASSERT(imageLabel->pixmap());
  scaleFactor *= factor;
  imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

  adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(scrollArea->verticalScrollBar(), factor);

  zoomInAct->setEnabled(scaleFactor < 3.0);
  zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value()
        + ((factor - 1) * scrollBar->pageStep()/2)));
}
