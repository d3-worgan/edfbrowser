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




#include "annotlist_filter_dialog.h"



UI_AnnotFilterWindow::UI_AnnotFilterWindow(QWidget *w_parent, struct annotationblock *annot)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  annot_filter_dialog = new QDialog(w_parent);

  annot_list = &mainwindow->edfheaderlist[0]->annot_list;

  annots_dock = mainwindow->annotations_dock[0];

  annot_filter_dialog->setMinimumSize(600, 400);
  annot_filter_dialog->setMaximumSize(600, 400);
  annot_filter_dialog->setWindowTitle("Filter annotations");
  annot_filter_dialog->setModal(true);
  annot_filter_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  annotLabel = new QLabel(annot_filter_dialog);
  annotLabel->setGeometry(20, 20, 100, 25);
  annotLabel->setText("Filter annotation:");

  annotNameLabel = new QLabel(annot_filter_dialog);
  annotNameLabel->setGeometry(130, 20, 200, 25);

  t1_dspinbox = new QDoubleSpinBox(annot_filter_dialog);
  t1_dspinbox->setGeometry(20, 250, 100, 25);
  t1_dspinbox->setDecimals(3);
  t1_dspinbox->setRange(0.01, 300.0);
  t1_dspinbox->setSuffix(" sec");
  t1_dspinbox->setValue(1);

  t2_dspinbox = new QDoubleSpinBox(annot_filter_dialog);
  t2_dspinbox->setGeometry(130, 250, 100, 25);
  t2_dspinbox->setDecimals(3);
  t2_dspinbox->setRange(0.01, 300.0);
  t2_dspinbox->setSuffix(" sec");
  t2_dspinbox->setValue(2);

  ApplyButton = new QPushButton(annot_filter_dialog);
  ApplyButton->setGeometry(20, 355, 100, 25);
  ApplyButton->setText("Apply");

  CloseButton = new QPushButton(annot_filter_dialog);
  CloseButton->setGeometry(310, 355, 100, 25);
  CloseButton->setText("Close");

  sel_annot_ptr = annot;
  strcpy(sel_annot_str, sel_annot_ptr->annotation);
  remove_leading_spaces(sel_annot_str);
  remove_trailing_spaces(sel_annot_str);

  annotNameLabel->setText(sel_annot_str);

  QObject::connect(ApplyButton, SIGNAL(clicked()), this,                SLOT(apply_filter()));
  QObject::connect(CloseButton, SIGNAL(clicked()), annot_filter_dialog, SLOT(close()));

  annot_filter_dialog->exec();
}


void UI_AnnotFilterWindow::apply_filter()
{
  int i, sz, is_set=0;

  long long t1, t_diff, t_min, t_max;

  char annot_str[MAX_ANNOTATION_LEN + 1];

  struct annotationblock *annot, *annot_before;

  t_min = t1_dspinbox->value() * TIME_DIMENSION;

  t_max = t2_dspinbox->value() * TIME_DIMENSION;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strcpy(annot_str, annot->annotation);
    remove_leading_spaces(sel_annot_str);
    remove_trailing_spaces(annot_str);

    if(strcmp(annot_str, sel_annot_str))
    {
      annot->hided_in_list = 1;

      annot->hided = 1;

      continue;
    }

    if(is_set == 0)
    {
      is_set = 1;

      t1 = annot->onset;

      annot->hided_in_list = 1;

      annot->hided = 1;

      annot_before = annot;

      continue;
    }

    t_diff = annot->onset - t1;

    t1 = annot->onset;

    if((t_diff >= t_min) && (t_diff <= t_max))
    {
      annot->hided_in_list = 1;

      annot->hided = 1;
    }
    else
    {
      annot->hided_in_list = 0;

      annot->hided = 0;

      annot_before->hided_in_list = 0;

      annot_before->hided = 0;
    }

    annot_before = annot;
  }

  annots_dock->updateList();

  mainwindow->maincurve->update();
}












