/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 Teunis van Beelen
*
* Email: teuniz@gmail.com
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




#ifndef ANNOTFILTER_FORM1_H
#define ANNOTFILTER_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QProgressDialog>
#include <QComboBox>
#include <QList>
#include <QTime>
#include <QTimeEdit>
#include <QDoubleSpinBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "annotations_dock.h"



class UI_Mainwindow;

class UI_Annotationswindow;



class UI_AnnotFilterWindow : public QObject
{
  Q_OBJECT

public:
  UI_AnnotFilterWindow(QWidget *, struct annotationblock *);

  UI_Mainwindow *mainwindow;


private:

QDialog     *annot_filter_dialog;

QLabel      *annotLabel,
            *annotNameLabel;
//             *signalLabel,
//             *ratioLabel,
//             *bufsizeLabel,
//             *time1Label,
//             *time2Label;

QPushButton *CloseButton,
            *ApplyButton;

QListWidget *list;

// QComboBox   *ratioBox;
//
// QTimeEdit    *timeEdit1,
//              *timeEdit2;
//
// QTime        time1,
//              time2;

QDoubleSpinBox *t1_dspinbox,
               *t2_dspinbox;

char sel_annot_str[MAX_ANNOTATION_LEN + 1];

struct annotation_list *annot_list;

struct annotationblock *sel_annot_ptr;

UI_Annotationswindow *annots_dock;

private slots:

void apply_filter();

};



#endif // ANNOTFILTER_FORM1_H










