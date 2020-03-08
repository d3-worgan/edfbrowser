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


#ifndef UI_HISTOGRAMDOCKFORM_H
#define UI_HISTOGRAMDOCKFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QFrame>
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
#include <QRectF>
#include <QPoint>
#include <QToolBar>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"



class UI_Mainwindow;
class simple_tracking_indicator2;
class simple_ruler_indicator2;



struct histogram_dock_param_struct
{
  int instance_num;
  int file_num;
  char stage_name[5][32];
  char annot_name[5][32];
};


class UI_histogram_dock : public QObject
{
  Q_OBJECT

public:
  UI_histogram_dock(QWidget *, struct histogram_dock_param_struct);
  ~UI_histogram_dock();

  UI_Mainwindow  *mainwindow;

  QToolBar *histogram_dock;

private:

  struct histogram_dock_param_struct param;

  simple_tracking_indicator2 *trck_indic;

  simple_ruler_indicator2 *srl_indic;

  int is_deleted;

private slots:

  void file_pos_changed(long long);
  void hide_histogram_dock(bool);

};


class simple_tracking_indicator2: public QWidget
{
  Q_OBJECT

public:
  simple_tracking_indicator2(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_position(long long);
  void set_maximum(long long);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  long long pos, max;

  void draw_small_arrow(QPainter *, int, int, int, QColor);
};


class simple_ruler_indicator2: public QWidget
{
  Q_OBJECT

public:
  simple_ruler_indicator2(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_minimum(int);
  void set_maximum(int);
  void set_unit(const char *);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  char unit[32];

  int min, max;
};

#endif










