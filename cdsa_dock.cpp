/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/



#include "cdsa_dock.h"


// class simple_tracking_indicator: public QWidget
// {
//   Q_OBJECT
//
// public:
//   simple_tracking_indicator(QWidget *parent=0);
//   ~simple_tracking_indicator();
//
//   QSize sizeHint() const {return minimumSizeHint(); }
//   QSize minimumSizeHint() const {return QSize(5, 5); }
//
// public slots:
//
// protected:
//   void paintEvent(QPaintEvent *);
//
// private:
//
//   QFont *trck_font;
// };


UI_cdsa_dock::UI_cdsa_dock(QWidget *w_parent, struct cdsa_dock_param_struct par)
{
  char str[1024]={""};

  QLabel *cdsa_label=NULL;

//   QFrame *frame;

  QDockWidget *cdsa_dock=NULL;

//   simple_tracking_indicator *trck_indic;

//   QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  param = par;

  snprintf(str, 1024, "Color Density Spectral Array   %s", param.signalcomp->signallabel);

  cdsa_dock = new QDockWidget(str, mainwindow);

  cdsa_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);

  cdsa_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

//   frame = new QFrame;

  cdsa_label = new QLabel;
  cdsa_label->setScaledContents(true);
  cdsa_label->setPixmap(*param.pxm);
  cdsa_label->setMinimumHeight(100);
  cdsa_label->setMinimumWidth(100);

//   trck_indic = new simple_tracking_indicator;

//   grid_layout = new QGridLayout(frame);
//   grid_layout->addWidget(cdsa_label, 0, 0);
//   grid_layout->addWidget(trck_indic, 1, 0);

  cdsa_dock->setWidget(cdsa_label);

  mainwindow->addDockWidget(Qt::BottomDockWidgetArea, cdsa_dock, Qt::Horizontal);

  QObject::connect(cdsa_dock, SIGNAL(destroyed(QObject *)), this, SLOT(cdsa_dock_destroyed(QObject *)));
}


UI_cdsa_dock::~UI_cdsa_dock()
{
  delete param.pxm;
}


void UI_cdsa_dock::cdsa_dock_destroyed(QObject *)
{
  delete this;
}


// simple_tracking_indicator::simple_tracking_indicator(QWidget *w_parent) : QWidget(w_parent)
// {
//   setAttribute(Qt::WA_OpaquePaintEvent);
//
//   trck_font = new QFont;
// #ifdef Q_OS_WIN32
//   trck_font->setFamily("Tahoma");
//   trck_font->setPixelSize(11);
// #else
//   trck_font->setFamily("Arial");
//   trck_font->setPixelSize(12);
// #endif
// }
//
//
// simple_tracking_indicator::~simple_tracking_indicator()
// {
//   delete trck_font;
// }
//
//
// void simple_tracking_indicator::paintEvent(QPaintEvent *)
// {
//   int w, h;
//
//   w = width();
//   h = height();
//
//   QPainter painter(this);
//
//   painter.fillRect(0, 0, w, h, Qt::green);
//
// }































