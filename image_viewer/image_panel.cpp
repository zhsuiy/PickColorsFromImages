#include "image_panel.h"

void ImagePanel::mouseReleaseEvent(QMouseEvent* event) {
  QVector2D pos = QVector2D(event->localPos());
  auto image = this->pixmap()->toImage();
  QColor color(image.pixel(pos.x(), pos.y()));
  emit Clicked(color); 
}
