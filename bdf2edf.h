/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2020 Teunis van Beelen
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


#ifndef UI_BDF2EDFFORM_H
#define UI_BDF2EDFFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QFileDialog>
#include <QCursor>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "check_edf_file.h"
#include "filter.h"
#include "utils.h"



class UI_Mainwindow;



class UI_BDF2EDFwindow : public QObject
{
  Q_OBJECT

public:

  UI_BDF2EDFwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

QLabel       *label1;

QPushButton  *pushButton1,
             *pushButton2,
             *pushButton3,
             *pushButton4,
             *pushButton5;

QDoubleSpinBox *spinBox1,
               *spinBox2;

QTableWidget *SignalsTablewidget;

QDialog      *myobjectDialog;

int  signalslist[MAXSIGNALS],
     annotlist[MAXSIGNALS];


char  inputpath[MAX_PATH_LENGTH],
      outputpath[MAX_PATH_LENGTH],
      *recent_opendir,
      *recent_savedir;

double dividerlist[MAXSIGNALS];

FILE *inputfile,
     *outputfile;

struct edfhdrblock *edfhdr;

struct filter_settings *filterlist[MAXSIGNALS];

void showpopupmessage(const char *, const char *);

private slots:

void SelectFileButton();
void StartConversion();
void spinbox1_changed(double);
void spinbox2_changed(double);
void Select_all_signals();
void Deselect_all_signals();
void free_edfheader();

};




#endif


