#include <QtGui>

#include "scrubber.h"

Scrubber::Scrubber(QWidget *parent)
    : QWidget(parent)
{
  current_item_ = 0;
  frame_change = 0;
}

void Scrubber::setNumItems(int newsize) {
  num_items_ = newsize;
  items_.resize(num_items_);
  clearItems();
}

void Scrubber::setItem(int index, QPixmap &pix){
  items_[index] = pix;
}

void Scrubber::setCurrentItem(int index) {
  current_item_ = index;
}

void Scrubber::clearItems() {
  for (unsigned int i = 0; i < items_.size(); ++i) {
    items_[i] = QPixmap();
  }

}
QPixmap Scrubber::GetItem(int index) {
  return items_[index];
}

QPixmap Scrubber::GetCurrentItem() {
  return items_[current_item_];
}

void Scrubber::setCurrentItem(QPixmap pix) {
  items_[current_item_] = pix;
}

// TODO(keir): This breaks down when there are more than about 30 frames,
// because the names run into each other. Change this so that there is a mode
// that only shows 1 .. 10 .. 20 .. instead of every frame.
void Scrubber::paintEvent(QPaintEvent *)
{
    QColor itemPresentColor(100, 205, 100, 191);
    QColor itemHighlightedColor(100, 5, 20, 191);

    QPainter painter(this);
    int blockWidth = width() / num_items_;
    int blockHeight = height();
    painter.setFont(QFont("Arial", 8));

    QRect itemRect(blockWidth*current_item_, 0, blockWidth, blockHeight);
    painter.fillRect(itemRect, itemHighlightedColor);

    for (unsigned int i = 0; i < items_.size(); ++i) {
      painter.setPen(Qt::gray);
      painter.drawLine(blockWidth*i, 0, blockWidth*i, blockHeight);
      painter.setPen(Qt::black);
      QRect itemRect(blockWidth*i, 0, blockWidth, blockHeight);
      if (!items_[i].isNull()) {
        painter.fillRect(itemRect, itemPresentColor);
      }
      QString numformat;
      numformat.sprintf("%d", i);
      painter.drawText(itemRect, Qt::AlignCenter, numformat);
    }
}

void Scrubber::mousePressEvent (QMouseEvent *e) {
  int blockWidth = width() / num_items_;
  int itemHit = e->x() / blockWidth;
  setCurrentItem(itemHit);
  repaint();
  emit selectionChanged(itemHit);
  if(frame_change)
    frame_change(frame_change_data_);
}
