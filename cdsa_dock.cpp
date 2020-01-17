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

  QDockWidget *cdsa_dock=NULL;

  mainwindow = (UI_Mainwindow *)w_parent;

  param = par;

  snprintf(str, 1024, "Color Density Spectral Array   %s", param.signalcomp->signallabel);

  cdsa_dock = new QDockWidget(str, mainwindow);

  cdsa_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);

  cdsa_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

  cdsa_label = new QLabel(cdsa_dock);
  cdsa_label->setScaledContents(true);
  cdsa_label->setPixmap(*param.pxm);
  cdsa_label->setMinimumHeight(100);
  cdsa_label->setMinimumWidth(100);

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































