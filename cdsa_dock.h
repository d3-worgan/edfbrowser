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


#ifndef UI_CDSADOCKFORM_H
#define UI_CDSADOCKFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QObject>
#include <QFileDialog>
#include <QCheckBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QProgressDialog>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDockWidget>
#include <QGridLayout>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "filteredblockread.h"
#include "fft_wrap.h"



class UI_Mainwindow;


struct cdsa_dock_param_struct
{
  struct signalcompblock *signalcomp;
  int sf;
  int min_hz;
  int max_hz;
  double max_pwr;
  int log;
  QPixmap *pxm;
};


class UI_cdsa_dock : public QObject
{
  Q_OBJECT

public:
  UI_cdsa_dock(QWidget *, struct cdsa_dock_param_struct);
  ~UI_cdsa_dock();

  UI_Mainwindow  *mainwindow;

private:

  struct cdsa_dock_param_struct param;

private slots:

  void cdsa_dock_destroyed(QObject *);

};

#endif










