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



#include "cdsa_dialog.h"



UI_cdsa_window::UI_cdsa_window(QWidget *w_parent, struct signalcompblock *signal_comp)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signal_comp;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Color Density Spectral Array");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

//   pushButton1 = new QPushButton(myobjectDialog);
//   pushButton1->setGeometry(20, 430, 100, 25);
//   pushButton1->setText("Select File");
//
//   pushButton2 = new QPushButton(myobjectDialog);
//   pushButton2->setGeometry(480, 430, 100, 25);
//   pushButton2->setText("Close");

//   QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
//   QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


























