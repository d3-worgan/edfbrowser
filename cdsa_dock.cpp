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


UI_cdsa_dock::UI_cdsa_dock(QWidget *w_parent, struct cdsa_dock_param_struct par)
{
  char str[1024]={""};

  QLabel *cdsa_label=NULL;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  param = par;

  sigcomp_uid = param.signalcomp->uid;

  snprintf(str, 1024, "Color Density Spectral Array   %s", param.signalcomp->signallabel);

  cdsa_dock = new QDockWidget(str, mainwindow);

  cdsa_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);

  cdsa_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);

  cdsa_label = new QLabel;
  cdsa_label->setScaledContents(true);
  cdsa_label->setPixmap(*param.pxm);
  cdsa_label->setMinimumHeight(100);
  cdsa_label->setMinimumWidth(100);

  trck_indic = new simple_tracking_indicator;
  trck_indic->set_maximum((long long)param.segments_in_recording * (long long)param.segment_len * 10000000LL);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(cdsa_label, 0, 0);
  grid_layout->addWidget(trck_indic, 1, 0);

  cdsa_dock->setWidget(frame);

  mainwindow->addDockWidget(Qt::BottomDockWidgetArea, cdsa_dock, Qt::Horizontal);

  QObject::connect(cdsa_dock,  SIGNAL(destroyed(QObject *)),             this, SLOT(cdsa_dock_destroyed(QObject *)));
  QObject::connect(mainwindow, SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));

  file_pos_changed(0);

  delete param.pxm;
}


UI_cdsa_dock::~UI_cdsa_dock()
{
  cdsa_dock->close();
}


void UI_cdsa_dock::cdsa_dock_destroyed(QObject *)
{
  delete this;
}


void UI_cdsa_dock::file_pos_changed(long long)
{
  int i;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->uid == sigcomp_uid)
    {
      if(param.signalcomp == mainwindow->signalcomp[i])
      {
        break;
      }
    }
  }

  if(i == mainwindow->signalcomps)  return;

  trck_indic->set_position(param.signalcomp->edfhdr->viewtime + (mainwindow->pagetime / 2));
}


simple_tracking_indicator::simple_tracking_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

  trck_font = new QFont;
#ifdef Q_OS_WIN32
  trck_font->setFamily("Tahoma");
  trck_font->setPixelSize(11);
#else
  trck_font->setFamily("Arial");
  trck_font->setPixelSize(12);
#endif

  pos = 0;
  max = 100;
}


void simple_tracking_indicator::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void simple_tracking_indicator::set_maximum(long long max_)
{
  max = max_;
}


simple_tracking_indicator::~simple_tracking_indicator()
{
  delete trck_font;
}


void simple_tracking_indicator::paintEvent(QPaintEvent *)
{
  int w, h;

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  draw_small_arrow(&painter, (int)(((double)pos / (double)max) * w), 0, 0, Qt::black);

}


void simple_tracking_indicator::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
{
  QPainterPath path;

  if(rot == 0)
  {
    path.moveTo(xpos,      ypos);
    path.lineTo(xpos - 10, ypos + 15);
    path.lineTo(xpos + 10, ypos + 15);
    path.lineTo(xpos,      ypos);

    painter->fillPath(path, color);
  }
}































