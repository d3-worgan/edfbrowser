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



#include "hypnogram_dialog.h"





UI_hypnogram_window::UI_hypnogram_window(QWidget *w_parent, int f_num, int i_num)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  instance_num = i_num;

  file_num = f_num;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 525);
  myobjectDialog->setMaximumSize(600, 525);
  myobjectDialog->setWindowTitle("Hypnogram");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  stage_label = new QLabel(myobjectDialog);
  stage_label->setGeometry(20, 20, 150, 25);
  stage_label->setText("Stage");

  annot_label = new QLabel(myobjectDialog);
  annot_label->setGeometry(200, 20, 150, 25);
  annot_label->setText("Annotation");

  stage1_edit = new QLineEdit(myobjectDialog);
  stage1_edit->setGeometry(20, 65, 150, 25);
  stage2_edit = new QLineEdit(myobjectDialog);
  stage2_edit->setGeometry(20, 110, 150, 25);
  stage3_edit = new QLineEdit(myobjectDialog);
  stage3_edit->setGeometry(20, 155, 150, 25);
  stage4_edit = new QLineEdit(myobjectDialog);
  stage4_edit->setGeometry(20, 200, 150, 25);
  stage5_edit = new QLineEdit(myobjectDialog);
  stage5_edit->setGeometry(20, 245, 150, 25);
  stage6_edit = new QLineEdit(myobjectDialog);
  stage6_edit->setGeometry(20, 290, 150, 25);

  annot1_edit = new QLineEdit(myobjectDialog);
  annot1_edit->setGeometry(200, 65, 150, 25);
  annot2_edit = new QLineEdit(myobjectDialog);
  annot2_edit->setGeometry(200, 110, 150, 25);
  annot3_edit = new QLineEdit(myobjectDialog);
  annot3_edit->setGeometry(200, 155, 150, 25);
  annot4_edit = new QLineEdit(myobjectDialog);
  annot4_edit->setGeometry(200, 200, 150, 25);
  annot5_edit = new QLineEdit(myobjectDialog);
  annot5_edit->setGeometry(200, 245, 150, 25);
  annot6_edit = new QLineEdit(myobjectDialog);
  annot6_edit->setGeometry(200, 290, 150, 25);

  close_button = new QPushButton(myobjectDialog);
  close_button->setGeometry(20, 470, 100, 25);
  close_button->setText("Close");

  default_button = new QPushButton(myobjectDialog);
  default_button->setGeometry(250, 470, 100, 25);
  default_button->setText("Default");

  start_button = new QPushButton(myobjectDialog);
  start_button->setGeometry(480, 470, 100, 25);
  start_button->setText("Start");

  stage1_edit->setText(mainwindow->hypnogram_stage_name[0]);
  stage2_edit->setText(mainwindow->hypnogram_stage_name[1]);
  stage3_edit->setText(mainwindow->hypnogram_stage_name[2]);
  stage4_edit->setText(mainwindow->hypnogram_stage_name[3]);
  stage5_edit->setText(mainwindow->hypnogram_stage_name[4]);
  stage6_edit->setText(mainwindow->hypnogram_stage_name[5]);

  annot1_edit->setText(mainwindow->hypnogram_annot_name[0]);
  annot2_edit->setText(mainwindow->hypnogram_annot_name[1]);
  annot3_edit->setText(mainwindow->hypnogram_annot_name[2]);
  annot4_edit->setText(mainwindow->hypnogram_annot_name[3]);
  annot5_edit->setText(mainwindow->hypnogram_annot_name[4]);
  annot6_edit->setText(mainwindow->hypnogram_annot_name[5]);

  QObject::connect(close_button,         SIGNAL(clicked()),  myobjectDialog, SLOT(close()));
  QObject::connect(default_button,       SIGNAL(clicked()),            this, SLOT(default_button_clicked()));
  QObject::connect(start_button,         SIGNAL(clicked()),            this, SLOT(start_button_clicked()));

  myobjectDialog->exec();
}


void UI_hypnogram_window::default_button_clicked()
{
  stage1_edit->setText("W");
  stage2_edit->setText("R");
  stage3_edit->setText("N1");
  stage4_edit->setText("N2");
  stage5_edit->setText("N3");
  stage6_edit->setText("N4");

  annot1_edit->setText("W");
  annot2_edit->setText("R");
  annot3_edit->setText("N1");
  annot4_edit->setText("N2");
  annot5_edit->setText("N3");
  annot6_edit->setText("N4");
}


void UI_hypnogram_window::start_button_clicked()
{
  struct hypnogram_dock_param_struct dock_param;

  memset(&dock_param, 0, sizeof(struct hypnogram_dock_param_struct));

  dock_param.instance_num = instance_num;

  dock_param.file_num = file_num;

  dock_param.mainwindow = mainwindow;

  strlcpy(dock_param.stage_name[0], stage1_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[1], stage2_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[2], stage3_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[3], stage4_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[4], stage5_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[5], stage6_edit->text().toLatin1().data(), 32);

  strlcpy(dock_param.annot_name[0], annot1_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[1], annot2_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[2], annot3_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[3], annot4_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[4], annot5_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[5], annot6_edit->text().toLatin1().data(), 32);

  strlcpy(mainwindow->hypnogram_stage_name[0], dock_param.stage_name[0], 32);
  strlcpy(mainwindow->hypnogram_stage_name[1], dock_param.stage_name[1], 32);
  strlcpy(mainwindow->hypnogram_stage_name[2], dock_param.stage_name[2], 32);
  strlcpy(mainwindow->hypnogram_stage_name[3], dock_param.stage_name[3], 32);
  strlcpy(mainwindow->hypnogram_stage_name[4], dock_param.stage_name[4], 32);
  strlcpy(mainwindow->hypnogram_stage_name[5], dock_param.stage_name[5], 32);

  strlcpy(mainwindow->hypnogram_annot_name[0], dock_param.annot_name[0], 32);
  strlcpy(mainwindow->hypnogram_annot_name[1], dock_param.annot_name[1], 32);
  strlcpy(mainwindow->hypnogram_annot_name[2], dock_param.annot_name[2], 32);
  strlcpy(mainwindow->hypnogram_annot_name[3], dock_param.annot_name[3], 32);
  strlcpy(mainwindow->hypnogram_annot_name[4], dock_param.annot_name[4], 32);
  strlcpy(mainwindow->hypnogram_annot_name[5], dock_param.annot_name[5], 32);

  mainwindow->hypnogram_dock[instance_num] = new UI_hypnogram_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->hypnogram_dock[instance_num]->hypnogram_dock);

  mainwindow->insertToolBarBreak(mainwindow->hypnogram_dock[instance_num]->hypnogram_dock);

  mainwindow->edfheaderlist[file_num]->hypnogram_dock[instance_num] = instance_num + 1;

  myobjectDialog->close();
}





























