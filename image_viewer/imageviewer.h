#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QColor;
class ImagePanel;
QT_END_NAMESPACE

class ImageViewer : public QMainWindow
{
  Q_OBJECT

public:
  ImageViewer();
  bool loadFile(const QString &);

  private slots:
    void open();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();
    void PrevImg();
    void NextImg();
    void ClearLastSelectedColor();
    void SelectNewColor(QColor color);
    void AddToGoodColorSet();

private:
  void createActions();
  void createMenus();
  void updateActions();
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar *scrollBar, double factor);
  void ClearAllSelectedColors();
  void SaveToFile(const QVector<QColor> &selectedColors);

private:
  /*  */
  ImagePanel *imageLabel;
  QScrollArea *scrollArea;
  double scaleFactor;

  QString imgFolderDir;
  QStringList imgPathList;
  int imgCurrentIndex;

  enum { MaxColorNum = 5 };
  QVector<QColor> selectedColors;
  QVector<QLabel*> colorLabels;
  QVector<QVector<QColor>> goodColorSetVector;

  /*  */
  QAction *openAct;
  QAction *exitAct;
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *normalSizeAct;
  QAction *fitToWindowAct;
  QAction *aboutAct;
  QAction *aboutQtAct;


  /*  */
  QAction *prevImgAct;
  QAction *nextImgAct;
  QAction *clearLastSelectedColor;
  QAction *addToGoodColorSet;

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;
};

#endif
