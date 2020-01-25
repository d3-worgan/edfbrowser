

#include <QApplication>
#include <QObject>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QStyle>
#include <QStyleFactory>
#include <QPen>
#include <QColor>
#include <QFont>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

//   app.setAttribute(Qt::AA_DontUseNativeMenuBar);

  QPixmap pixmap(":/images/splash.png");

  QPainter p(&pixmap);
  QFont sansFont("Noto Sans", 10);
  p.setFont(sansFont);
  p.setPen(Qt::black);
  if(!strcmp(PROGRAM_BETA_SUFFIX, ""))
  {
    p.drawText(250, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "    " THIS_APP_BITS_W);
  }
  else
  {
    p.drawText(150, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION " " PROGRAM_BETA_SUFFIX "    " THIS_APP_BITS_W);
  }

  QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

  QTimer t1;
  t1.setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1.setTimerType(Qt::PreciseTimer);
#endif
  QObject::connect(&t1, SIGNAL(timeout()), &splash, SLOT(close()));

  splash.show();

  t1.start(3000);

  qApp->processEvents();

#if QT_VERSION >= 0x050000
#ifdef Q_OS_LINUX
  qApp->setStyle(QStyleFactory::create("Fusion"));
#endif
#endif
  qApp->setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");

  class UI_Mainwindow MainWindow;

  return app.exec();
}





