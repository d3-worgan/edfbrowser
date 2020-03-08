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



#include "hypnogram_dock.h"


UI_hypnogram_dock::UI_hypnogram_dock(QWidget *w_parent, struct hypnogram_dock_param_struct par)
{
  QLabel *hypnogram_label,
         *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  param = par;

  is_deleted = 0;

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  hypnogram_label = new QLabel;
  hypnogram_label->setMinimumHeight(100);
  hypnogram_label->setMinimumWidth(100);
  hypnogram_label->setContentsMargins(0, 0, 0, 0);
  hypnogram_label->setText("test1234");
  hypnogram_label->setStyleSheet("QLabel { background-color : #c0c0c0; color : black; }");

  trck_indic = new simple_tracking_indicator2;
  trck_indic->set_maximum(mainwindow->edfheaderlist[param.file_num]->recording_len_sec);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new simple_ruler_indicator2;
  srl_indic->set_params(&param);
  srl_indic->setContentsMargins(0, 0, 0, 0);
  srl_indic->setMinimumWidth(80);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Stage / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(hypnogram_label, 0, 1);
  grid_layout->addWidget(ruler_label, 1, 0);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->setColumnStretch(1, 100);

  hypnogram_dock = new QToolBar("Hypnogram", mainwindow);
  hypnogram_dock->setOrientation(Qt::Horizontal);
  hypnogram_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  hypnogram_dock->addWidget(frame);

  QObject::connect(mainwindow,     SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));
  QObject::connect(hypnogram_dock, SIGNAL(visibilityChanged(bool)),          this, SLOT(hide_hypnogram_dock(bool)));
  QObject::connect(mainwindow,     SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));

  file_pos_changed(0);
}


UI_hypnogram_dock::~UI_hypnogram_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeToolBar(hypnogram_dock);

    mainwindow->edfheaderlist[param.file_num]->hypnogram_dock[param.instance_num] = 0;

    mainwindow->hypnogram_dock[param.instance_num] = NULL;
  }
}


void UI_hypnogram_dock::hide_hypnogram_dock(bool visible)
{
  if(visible == false)
  {
    if(!is_deleted)
    {
      delete this;
    }
  }
}


void UI_hypnogram_dock::file_pos_changed(long long)
{
  trck_indic->set_position((int)((mainwindow->edfheaderlist[param.file_num]->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
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


simple_ruler_indicator2::simple_ruler_indicator2(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);
}


void simple_ruler_indicator2::set_params(struct hypnogram_dock_param_struct *parms)
{
  param = *parms;
}


void simple_ruler_indicator2::paintEvent(QPaintEvent *)
{
  int i, w, h;

  double pixel_per_unit,
         offset;

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  pixel_per_unit = (double)h / 5.0;

  offset = (double)h / 10.0;

  painter.drawLine(w - 4, offset, w - 4, h - offset);

  for(i=0; i<5; i++)
  {
    painter.drawLine(w - 4, (int)((pixel_per_unit * i) + 0.5 + offset), w - 13, (int)((pixel_per_unit * i) + 0.5 + offset));

    painter.drawText(QRectF(2, (int)((pixel_per_unit * i) + 0.5 + offset) - 9, 60, 25), Qt::AlignRight | Qt::AlignHCenter, param.stage_name[i]);
  }
}





























