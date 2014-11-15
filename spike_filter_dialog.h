/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 Teunis van Beelen
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



#ifndef ADD_SPIKE_FILTERFORM1_H
#define ADD_SPIKE_FILTERFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QString>
#include <QMessageBox>
#include <QVariant>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "spike_filter.h"
#include "popup_messagewindow.h"
#include "utils.h"



class UI_Mainwindow;



class UI_SpikeFilterDialog : public QObject
{
  Q_OBJECT

public:
  UI_SpikeFilterDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *spikefilterdialog;

QPushButton    *CancelButton,
               *ApplyButton;

QListWidget    *list;

QLabel         *listlabel,
               *velocityLabel,
               *holdOffLabel;

QDoubleSpinBox *velocitySpinBox;

QSpinBox       *holdOffSpinBox;


private slots:

void ApplyButtonClicked();

};



#endif // ADD_SPIKE_FILTERFORM1_H








