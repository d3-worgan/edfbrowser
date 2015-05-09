/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/


#ifndef UI_FMA2EDFFORM_H
#define UI_FMA2EDFFORM_H


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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "popup_messagewindow.h"
#include "utils.h"
#include "edflib.h"
#include "ravg_filter.h"




class UI_FMaudio2EDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_FMaudio2EDFwindow(char *recent_dir=NULL, char *save_dir=NULL);

private:

QDialog       *myobjectDialog;

QPushButton   *pushButton1,
              *pushButton2;

QLabel        *PatientnameLabel,
              *RecordingLabel,
              *DatetimeLabel;

QLineEdit     *PatientnameLineEdit,
              *RecordingLineEdit;

QDateTimeEdit *StartDatetimeedit;

char *recent_opendir,
     *recent_savedir;

struct ravg_filter_settings *hpf44,
                            *lpf9a,
                            *lpf9b,
                            *lpf9c,
                            *lpf9d,
                            *lpf9e,
                            *lpf200a,
                            *lpf200b,
                            *lpf200c,
                            *lpf200d,
                            *lpf200e;

void enable_widgets(bool);

bool allocateFilters(int, int, int);
void deleteFilters(void);

private slots:

void SelectFileButton();

};




#endif


