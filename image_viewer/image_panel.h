#ifndef IMAGE_PANEL_H_
#define IMAGE_PANLE_H_

#include <QtWidgets>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QColor;
QT_END_NAMESPACE

class ImagePanel : public QLabel {
  Q_OBJECT

public:

signals:
  void Clicked(QColor color);

protected:
  void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:

};

#endif