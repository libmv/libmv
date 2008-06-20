#ifndef SRC_UI_SCRUBBER_H
#define SRC_UI_SCRUBBER_H

#include <QWidget>
#include <vector>

class Scrubber : public QWidget
{
  Q_OBJECT

 public:
  Scrubber(QWidget *parent = 0);
  void setNumItems(int newsize);
  void setItem(int index, char flag);
  void setCurrentItem(int index);
  void clearItems();

 protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent (QMouseEvent *e);

 signals:
  void selectionChanged(int newValue);

 private:
  int num_items_;
  std::vector<char> items_;
  int current_item_;
};

#endif  // SRC_UI_SCRUBBER_H
