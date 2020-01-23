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

  QLabel *cdsa_label,
         *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  param = par;

  is_deleted = 0;

  sigcomp_uid = param.signalcomp->uid;

  snprintf(str, 1024, "Color Density Spectral Array   %s", param.signalcomp->signallabel);

  cdsa_dock = new QDockWidget(str, mainwindow);

  cdsa_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);

  cdsa_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  cdsa_label = new QLabel;
  cdsa_label->setScaledContents(true);
  cdsa_label->setPixmap(*param.pxm);
  cdsa_label->setMinimumHeight(100);
  cdsa_label->setMinimumWidth(100);
  cdsa_label->setContentsMargins(0, 0, 0, 0);

  trck_indic = new simple_tracking_indicator;
  trck_indic->set_maximum(param.segments_in_recording * param.segment_len);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new simple_ruler_indicator;
  srl_indic->set_maximum(param.max_hz);
  srl_indic->set_minimum(param.min_hz);
  srl_indic->set_unit("Hz");
  srl_indic->setContentsMargins(0, 0, 0, 0);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Hz / position");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(cdsa_label, 0, 1);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->addWidget(ruler_label, 1, 0);

  cdsa_dock->setWidget(frame);

  mainwindow->addDockWidget(Qt::BottomDockWidgetArea, cdsa_dock, Qt::Horizontal);

//  QObject::connect(cdsa_dock,  SIGNAL(destroyed(QObject *)),             this, SLOT(cdsa_dock_destroyed(QObject *)));
  QObject::connect(cdsa_dock,  SIGNAL(visibilityChanged(bool)),          this, SLOT(cdsa_dock_visibility_changed(bool)));
  QObject::connect(mainwindow, SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));

  file_pos_changed(0);

  delete param.pxm;
}


UI_cdsa_dock::~UI_cdsa_dock()
{
  QObject::disconnect(cdsa_dock,  SIGNAL(visibilityChanged(bool)),          this, SLOT(cdsa_dock_visibility_changed(bool)));

  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeDockWidget(cdsa_dock);

    cdsa_dock->close();

    param.signalcomp->cdsa_dock[param.instance_nr] = 0;

    mainwindow->cdsa_dock[param.instance_nr] = NULL;
  }
}


void UI_cdsa_dock::cdsa_dock_destroyed(QObject *)
{
//   param.signalcomp->cdsa_dock[param.instance_nr] = 0;
//
//   mainwindow->cdsa_dock[param.instance_nr] = NULL;
//
//   printf("test 1\n");
//
//   delete this;
}


void UI_cdsa_dock::cdsa_dock_visibility_changed(bool visible)
{
  if(visible == true)  return;

  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeDockWidget(cdsa_dock);

    cdsa_dock->close();

    param.signalcomp->cdsa_dock[param.instance_nr] = 0;

    mainwindow->cdsa_dock[param.instance_nr] = NULL;
  }

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

  trck_indic->set_position((int)((param.signalcomp->edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


simple_tracking_indicator::simple_tracking_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

//   trck_font = new QFont;
// #ifdef Q_OS_WIN32
//   trck_font->setFamily("Tahoma");
//   trck_font->setPixelSize(11);
// #else
//   trck_font->setFamily("Arial");
//   trck_font->setPixelSize(12);
// #endif

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
//  delete trck_font;
}


void simple_tracking_indicator::paintEvent(QPaintEvent *)
{
  int w, h;

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  draw_small_arrow(&painter, (int)((((double)pos / (double)max) * w) + 0.5), 0, 0, Qt::black);
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


simple_ruler_indicator::simple_ruler_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(80);

//   rlr_font = new QFont;
// #ifdef Q_OS_WIN32
//   rlr_font->setFamily("Tahoma");
//   rlr_font->setPixelSize(11);
// #else
//   rlr_font->setFamily("Arial");
//   rlr_font->setPixelSize(12);
// #endif

  min = 0;
  max = 100;
}


simple_ruler_indicator::~simple_ruler_indicator()
{
//  delete rlr_font;
}


void simple_ruler_indicator::set_minimum(int val)
{
  min = val;
}


void simple_ruler_indicator::set_maximum(int val)
{
  max = val;
}


void simple_ruler_indicator::set_unit(const char *str)
{
  strlcpy(unit, str, 32);
}


void simple_ruler_indicator::paintEvent(QPaintEvent *)
{
  int i, w, h, range, skip;

  double pixel_per_unit;

  char str[64]={""};

  w = width();
  h = height();

  QPainter painter(this);

//  painter.setFont(*rlr_font);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  painter.drawLine(w - 4, 0, w - 4, h);

  range = max - min;

  pixel_per_unit = (double)h / (double)range;

  for(skip=1; skip<100000; )
  {
    if((skip * pixel_per_unit) > 25)  break;

    switch(skip)
    {
      case   1 : skip = 2;
                 break;
      case   2 : skip = 5;
                 break;
      case   5 : skip = 10;
                 break;
      case  10 : skip = 20;
                 break;
      case  20 : skip = 50;
                 break;
      case  50 : skip = 100;
                 break;
      case 100 : skip = 200;
                 break;
      case 200 : skip = 500;
                 break;
      case 500 : skip = 1000;
                 break;
      case 1000 : skip = 2000;
                 break;
      case 2000 : skip = 5000;
                 break;
      case 5000 : skip = 10000;
                 break;
      case 10000 : skip = 20000;
                 break;
      case 20000 : skip = 50000;
                 break;
      case 50000 : skip = 100000;
                 break;
    }
  }

  for(i=0; i<=range; i++)
  {
    if(!((i + min) % skip))
    {
      painter.drawLine(w - 4, h - (int)((pixel_per_unit * i) + 0.5), w - 13, h - (int)((pixel_per_unit * i) + 0.5));

      snprintf(str, 64, "%i", min + i);

      painter.drawText(QRectF(2, h - (int)((pixel_per_unit * i) + 0.5) - 7, 60, 25), Qt::AlignRight | Qt::AlignHCenter, str);
    }
  }
}





























