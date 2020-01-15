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


#ifndef UI_CDSAFORM_H
#define UI_CDSAFORM_H


#include <QtGlobal>
#include <QApplication>
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
#include <QDesktopServices>
#include <QUrl>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edflib.h"
#include "filteredblockread.h"



class UI_Mainwindow;


class UI_cdsa_window : public QObject
{
  Q_OBJECT

public:
  UI_cdsa_window(QWidget *, struct signalcompblock *);

  UI_Mainwindow  *mainwindow;

private:

  int sf;

  struct signalcompblock *signalcomp;

  QDialog       *myobjectDialog;

  QLabel        *windowlen_label,
                *blocklen_label,
                *overlap_label,
                *windowfunc_label,
                *pix_per_hz_label,
                *min_hz_label,
                *max_hz_label,
                *max_pwr_label;

  QSpinBox      *windowlen_spinbox,
                *blocklen_spinbox,
                *pix_per_hz_spinbox,
                *min_hz_spinbox,
                *max_hz_spinbox;

  QDoubleSpinBox *max_pwr_spinbox;

  QComboBox     *overlap_combobox,
                *windowfunc_combobox;

  QPushButton   *close_button,
                *start_button,
                *default_button;

private slots:

  void start_button_clicked();
  void default_button_clicked();
  void windowlen_spinbox_changed(int);
  void blocklen_spinbox_changed(int);
  void min_hz_spinbox_changed(int);
  void max_hz_spinbox_changed(int);

};




#endif


