/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 Teunis van Beelen
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



#ifndef ADD_FIR_FILTERFORM1_H
#define ADD_FIR_FILTERFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QMessageBox>
#include <QVariant>
#include <QTextEdit>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "fir_filter.h"
#include "utils.h"



class UI_Mainwindow;



class UI_FIRFilterDialog : public QObject
{
  Q_OBJECT

public:
  UI_FIRFilterDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *firfilterdialog;

QPushButton    *CancelButton,
               *ApplyButton;

QListWidget    *list;

QTextEdit      *textEdit;

QLabel         *listlabel,
               *varsLabel;


private slots:

void ApplyButtonClicked();

};



#endif // ADD_FIR_FILTERFORM1_H








