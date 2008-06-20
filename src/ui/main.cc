#include <QApplication>
#include "imageviewer.h"
#include "scrubber.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ImageViewer imageViewer;
    imageViewer.show();
    return app.exec();
}
