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
  char txt_string[2048];

  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Mortara XML to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  textEdit1 = new QTextEdit(myobjectDialog);
  textEdit1->setGeometry(20, 20, 560, 380);
  textEdit1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  sprintf(txt_string, "Mortara XML to EDF converter.\n");
  textEdit1->append(txt_string);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 430, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 430, 100, 25);
  pushButton2->setText("Close");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_MortaraEDFwindow::SelectFileButton()
{
  int err, char_encoding;

  char path[MAX_PATH_LENGTH],
       scratchpad[2048],
       outputfilename[MAX_PATH_LENGTH];

  struct xml_handle *xml_hdl;

  strcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "XML files (*.xml *.XML)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);

  if(xml_hdl == NULL)
  {
    sprintf(scratchpad, "Error, can not open file:\n%s\n", path);
    textEdit1->append(QString::fromLocal8Bit(scratchpad));
    return;
  }

  sprintf(scratchpad, "Processing file:\n%s", path);
  textEdit1->append(QString::fromLocal8Bit(scratchpad));

  char_encoding = xml_hdl->encoding;

  if(char_encoding == 0)  // attribute encoding not present
  {
    char_encoding = 2;  // fallback to UTF-8 because it is the default for XML
  }
  else if(char_encoding > 2)  // unknown encoding  FIX ME!!
  {
    char_encoding = 1;  // fallback to ISO-8859-1 (Latin1)
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "ECG"))
  {
    textEdit1->append("Error, can not find root element \"ECG\".\n");
    xml_close(xml_hdl);
    return;
  }

//   if(xml_goto_nth_element_inside(xml_hdl, "CHANNEL", 0))
//   {
//     textEdit1->append("Error, can not find element \"CHANNEL\".\n");
//     xml_close(xml_hdl);
//     return;
//   }

  err = xml_goto_nth_element_inside(xml_hdl, "TYPICAL_CYCLE", 0);
  if(err)
  {
    printf("error is: %i\n", err);
    textEdit1->append("Error, can not find element \"TYPICAL_CYCLE\".\n");
    xml_close(xml_hdl);
    return;
  }


  xml_close(xml_hdl);
}


void UI_MortaraEDFwindow::enable_widgets(bool toggle)
{
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}





















