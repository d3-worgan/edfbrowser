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
* the Free Software Foundation, version 3 of the License.
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



#include "histogram_dock.h"


UI_histogram_dock::UI_histogram_dock(QWidget *w_parent, struct histogram_dock_param_struct par)
{
  QLabel *histogram_label,
         *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  param = par;

  is_deleted = 0;

  frame = new QFrame;
//  frame->setMinimumSize(1050, 45);
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  histogram_label = new QLabel;
  histogram_label->setMinimumHeight(100);
  histogram_label->setMinimumWidth(100);
  histogram_label->setScaledContents(true);
  histogram_label->setContentsMargins(0, 0, 0, 0);
  histogram_label->setText("test1234");

  trck_indic = new simple_tracking_indicator2;
//FIXME  trck_indic->set_maximum(param.segments_in_recording * param.segment_len);
  trck_indic->set_maximum(2000);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Hz / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
//FIXME  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(histogram_label, 0, 1);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->addWidget(ruler_label, 1, 0);

  histogram_dock = new QToolBar("Histogram", mainwindow);
  histogram_dock->setOrientation(Qt::Horizontal);
  histogram_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  histogram_dock->addWidget(frame);

  QObject::connect(histogram_dock,  SIGNAL(destroyed(QObject *)),               this, SLOT(histogram_dock_destroyed(QObject *)));
  QObject::connect(mainwindow,      SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));

  file_pos_changed(0);
}


UI_histogram_dock::~UI_histogram_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    histogram_dock->close();

//FIXME    param.signalcomp->histogram_dock[param.instance_nr] = 0;

//FIXME    mainwindow->histogram_dock[param.instance_nr] = NULL;
  }
}


void UI_histogram_dock::histogram_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

//     param.signalcomp->histogram_dock[param.instance_nr] = 0;
//FIXME
//     mainwindow->histogram_dock[param.instance_nr] = NULL;
  }

  delete this;
}


void UI_histogram_dock::file_pos_changed(long long)
{
//   int i;
//
//   for(i=0; i<mainwindow->signalcomps; i++)
//   {
//     if(mainwindow->signalcomp[i]->uid == sigcomp_uid)
//     {
//       if(param.signalcomp == mainwindow->signalcomp[i])
//       {
//         break;
//       }
//     }
//   }
// FIXME
//   if(i == mainwindow->signalcomps)  return;
//
//   trck_indic->set_position((int)((param.signalcomp->edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


simple_tracking_indicator2::simple_tracking_indicator2(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

  pos = 0;
  max = 100;
}


void simple_tracking_indicator2::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void simple_tracking_indicator2::set_maximum(long long max_)
{
  max = max_;
}


void simple_tracking_indicator2::paintEvent(QPaintEvent *)
{
  int i, w, h, pos_x, step=0;

  char str[128]={""};

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  if(((double)w / (double)max) > 0.02)
  {
    step = 1;
  }
  else if(((double)w / (double)max) > 0.01)
    {
      step = 2;
    }
    else if(((double)w / (double)max) > 0.005)
      {
        step = 4;
      }

  if(step)
  {
    for(i=0; i<200; i+=step)
    {
      pos_x = (((double)(i * 3600) / (double)max) * (double)w) + 0.5;

      if(pos_x > w)
      {
        break;
      }

      painter.drawLine(pos_x, 0, pos_x, h);

      snprintf(str, 128, "%ih", i);

      painter.drawText(pos_x + 4, h - 2, str);
    }
  }

  draw_small_arrow(&painter, (int)((((double)pos / (double)max) * w) + 0.5), 0, 0, Qt::black);
}


void simple_tracking_indicator2::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
{
  QPainterPath path;

  if(rot == 0)
  {
    path.moveTo(xpos,      ypos);
    path.lineTo(xpos - 8, ypos + 15);
    path.lineTo(xpos + 8, ypos + 15);
    path.lineTo(xpos,      ypos);

    painter->fillPath(path, color);
  }
}































