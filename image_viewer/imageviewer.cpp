#include "imageviewer.h"
#include "image_panel.h"

#include <QtWidgets>

ImageViewer::ImageViewer()
{
  imageLabel = new ImagePanel;
  imageLabel->setBackgroundRole(QPalette::Base);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);
  connect(imageLabel, SIGNAL(Clicked(QColor)), this, SLOT(SelectNewColor(QColor)));

  scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(imageLabel);

  QWidget *sideWidget = new QWidget;
  QVBoxLayout *vLayout = new QVBoxLayout;
  for (int i = 0; i < MaxColorNum; ++i) {
    QLabel *newLabel = new QLabel;
    newLabel->setFixedSize(QSize(100, 50));
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::transparent);
    newLabel->setAutoFillBackground(true);
    newLabel->setPalette(sample_palette);
    colorLabels.push_back(newLabel);
    vLayout->addWidget(newLabel);
  }
  sideWidget->setLayout(vLayout);

  QHBoxLayout *hLayout = new QHBoxLayout;
  hLayout->addWidget(scrollArea);
  hLayout->addWidget(sideWidget);

  QWidget *mainWidget = new QWidget;
  mainWidget->setLayout(hLayout);

  setCentralWidget(mainWidget);

  createActions();
  createMenus();

  showMaximized();
}

bool ImageViewer::loadFile(const QString &fileName)
{
  QImageReader reader(fileName);
  reader.setAutoTransform(true);
  const QImage image = reader.read();
  if (image.isNull()) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
      tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
    setWindowFilePath(QString());
    imageLabel->setPixmap(QPixmap());
    imageLabel->adjustSize();
    return false;
  }
  imageLabel->setPixmap(QPixmap::fromImage(image));
  scaleFactor = 1.0;

  fitToWindowAct->setEnabled(true);
  updateActions();

  if (!fitToWindowAct->isChecked())
    imageLabel->adjustSize();

  setWindowFilePath(fileName);
  ClearAllSelectedColors();
  return true;
}

void ImageViewer::open()
{
  QStringList mimeTypeFilters;
  foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
    mimeTypeFilters.append(mimeTypeName);
  mimeTypeFilters.sort();
  const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  QFileDialog dialog(this, tr("Open File"), picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setMimeTypeFilters(mimeTypeFilters);
  dialog.selectMimeTypeFilter("image/jpeg");
  dialog.setFileMode(QFileDialog::ExistingFiles);

  if (dialog.exec() == QDialog::Accepted && dialog.selectedFiles().size() > 0) {
    imgCurrentIndex = 0;
    imgPathList = dialog.selectedFiles();
    imgFolderDir = QFileInfo(imgPathList[0]).absolutePath() + "/";
    loadFile(imgPathList[0]);
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

  prevImgAct = new QAction(tr("Prev Img"), this);
  prevImgAct->setShortcut(tr("A"));
  connect(prevImgAct, SIGNAL(triggered()), this, SLOT(PrevImg()));

  nextImgAct = new QAction(tr("Next Img"), this);
  nextImgAct->setShortcut(tr("D"));
  connect(nextImgAct, SIGNAL(triggered()), this, SLOT(NextImg()));

  clearLastSelectedColor = new QAction(tr("Clear Colors"), this);
  clearLastSelectedColor->setShortcut(tr("Q"));
  connect(clearLastSelectedColor, SIGNAL(triggered()), this, SLOT(ClearLastSelectedColor()));

  addToGoodColorSet = new QAction(tr("Add Color Set"), this);
  addToGoodColorSet->setShortcut(tr("Space")); 
  connect(addToGoodColorSet, SIGNAL(triggered()), this, SLOT(AddToGoodColorSet()));
}

void ImageViewer::createMenus()
{
  fileMenu = new QMenu(tr("&File"), this);
  fileMenu->addAction(openAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = new QMenu(tr("&View"), this);
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addAction(normalSizeAct);
  viewMenu->addSeparator();
  viewMenu->addAction(fitToWindowAct);
  viewMenu->addSeparator();
  viewMenu->addAction(prevImgAct);
  viewMenu->addAction(nextImgAct);
  viewMenu->addAction(clearLastSelectedColor);
  viewMenu->addAction(addToGoodColorSet);

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
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void ImageViewer::PrevImg() {
  imgCurrentIndex = (imgCurrentIndex + imgPathList.size() - 1) % imgPathList.size();
  loadFile(imgPathList[imgCurrentIndex]);
}

void ImageViewer::NextImg() {
  imgCurrentIndex = (imgCurrentIndex + 1) % imgPathList.size();
  loadFile(imgPathList[imgCurrentIndex]);
}

void ImageViewer::ClearLastSelectedColor() {
  if (selectedColors.size() == 0)
    return;

  QPalette sample_palette;
  sample_palette.setColor(QPalette::Window, Qt::transparent);
  colorLabels[selectedColors.size() - 1]->setPalette(sample_palette);
  selectedColors.removeLast();
}

void ImageViewer::SelectNewColor(QColor color) {
  if (selectedColors.size() >= MaxColorNum)
    return;
  selectedColors.push_back(color);

  QPalette sample_palette;
  sample_palette.setColor(QPalette::Window, color);
  colorLabels[selectedColors.size() - 1]->setPalette(sample_palette);
}

void ImageViewer::AddToGoodColorSet() {
  if (selectedColors.size() < MaxColorNum) {
    QMessageBox::warning(this, tr("Message"), tr("Not enough colors"));
  } else {
    goodColorSetVector.push_back(selectedColors);
    SaveToFile(selectedColors);
    QMessageBox::information(this, tr("Message"), tr("Color set saved"));
    NextImg();
  }
}

void ImageViewer::ClearAllSelectedColors() {
  while (selectedColors.size() > 0) {
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::transparent);
    colorLabels[selectedColors.size() - 1]->setPalette(sample_palette);
    selectedColors.removeLast();
  }
}

QString ColorStr(const QColor &color) {
  QString str = QString("(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
  return str;
}

QString ColorStrFloat(const QColor &color) {
  QString str = QString("(%1, %2, %3)").arg(color.redF()).arg(color.greenF()).arg(color.blueF());
  return str;
}

void ImageViewer::SaveToFile(const QVector<QColor> &selectedColors) {
  {
    QFile file(imgFolderDir + "colors.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
      QMessageBox::warning(this, tr("Message"), tr("Can not open file."));
      return;
    }
    QTextStream out(&file);
    QString colorStr;
    colorStr += "[";
    for (int i = 0; i < selectedColors.size(); ++i) {
      colorStr += ColorStr(selectedColors[i]);
      if (i != selectedColors.size() - 1) {
        colorStr += ", ";
      }
    }
    colorStr += "]";
    out << colorStr << endl;
  }
  {
    QFile file(imgFolderDir + "colors_float.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
      QMessageBox::warning(this, tr("Message"), tr("Can not open file."));
      return;
    }
    QTextStream out(&file);
    QString colorStr;
    colorStr += "[";
    for (int i = 0; i < selectedColors.size(); ++i) {
      colorStr += ColorStrFloat(selectedColors[i]);
      if (i != selectedColors.size() - 1) {
        colorStr += ", ";
      }
    }
    colorStr += "]";
    out << colorStr << endl;
  }
}