#ifndef SRC_UI_SCRUBBER_H
#define SRC_UI_SCRUBBER_H

#include <QWidget>
#include <vector>
#include <QPixmap>

class Scrubber : public QWidget
{
  Q_OBJECT

 public:
  Scrubber(QWidget *parent = 0);
  void setNumItems(int newsize);
  void setItem(int index, QPixmap &pix);
  void setCurrentItem(int index);
  void setCurrentItem(QPixmap pix);
  void clearItems();
  QPixmap GetItem(int index);
  QPixmap GetCurrentItem();
  
  void setCallback(void (*func)(void *), void *d) {
    frame_change = func;
    frame_change_data_ = d;
  }

 protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent (QMouseEvent *e);

 signals:
  void selectionChanged(int newValue);

 private:
  int num_items_;
  std::vector<QPixmap> items_;
  int current_item_;
  
  void (*frame_change)(void *);
  void *frame_change_data_;
};

#endif  // SRC_UI_SCRUBBER_H
