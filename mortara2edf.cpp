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



#include "mortara2edf.h"



UI_MortaraEDFwindow::UI_MortaraEDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 300);
  myobjectDialog->setMaximumSize(600, 300);
  myobjectDialog->setWindowTitle("Mortara XML to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 250, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 250, 100, 25);
  pushButton2->setText("Close");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_MortaraEDFwindow::SelectFileButton()
{

}


void UI_MortaraEDFwindow::enable_widgets(bool toggle)
{
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}





















