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



UI_AnnotFilterWindow::UI_AnnotFilterWindow(QWidget *w_parent, struct annotationblock *annot, struct annot_filter_struct *filter_p)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  annot_filter_dialog = new QDialog(w_parent);

  annot_list = &mainwindow->edfheaderlist[0]->annot_list;

  filter_params = filter_p;

  annots_dock = mainwindow->annotations_dock[0];

  annot_filter_dialog->setMinimumSize(600, 400);
  annot_filter_dialog->setMaximumSize(600, 400);
  annot_filter_dialog->setWindowTitle("Filter annotations");
  annot_filter_dialog->setModal(true);
  annot_filter_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  annotLabel = new QLabel(annot_filter_dialog);
  annotLabel->setGeometry(20, 20, 100, 25);
  annotLabel->setText("Filter annotation:");
  annotLabel->setToolTip("Label of the annotation to be filtered");

  annotNameLabel = new QLabel(annot_filter_dialog);
  annotNameLabel->setGeometry(160, 20, 200, 25);
  annotNameLabel->setToolTip("Label of the annotation to be filtered");

  t1Label = new QLabel(annot_filter_dialog);
  t1Label->setGeometry(20, 65, 120, 25);
  t1Label->setText("Minimum interval:");
  t1Label->setToolTip("Minimum interval between two consegutive annotations");

  t1_dspinbox = new QDoubleSpinBox(annot_filter_dialog);
  t1_dspinbox->setGeometry(160, 65, 100, 25);
  t1_dspinbox->setDecimals(3);
  t1_dspinbox->setRange(0.001, 299.999);
  t1_dspinbox->setSuffix(" sec");
  t1_dspinbox->setValue(filter_params->tmin);
  t1_dspinbox->setToolTip("Minimum interval between two consegutive annotations");

  t2Label = new QLabel(annot_filter_dialog);
  t2Label->setGeometry(20, 110, 120, 25);
  t2Label->setText("Maximum interval:");
  t2Label->setToolTip("Maximum interval between two consegutive annotations");

  t2_dspinbox = new QDoubleSpinBox(annot_filter_dialog);
  t2_dspinbox->setGeometry(160, 110, 100, 25);
  t2_dspinbox->setDecimals(3);
  t2_dspinbox->setRange(0.002, 300.0);
  t2_dspinbox->setSuffix(" sec");
  t2_dspinbox->setValue(filter_params->tmax);
  t2_dspinbox->setToolTip("Maximum interval between two consegutive annotations");

  invert_checkbox = new QCheckBox("Invert ", annot_filter_dialog);
  invert_checkbox->setGeometry(20, 155, 200, 25);
  invert_checkbox->setTristate(false);
  invert_checkbox->setToolTip("Invert the filter result");
  if(filter_params->invert)
  {
    invert_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    invert_checkbox->setCheckState(Qt::Unchecked);
  }

  hide_other_checkbox = new QCheckBox("Hide other annotations ", annot_filter_dialog);
  hide_other_checkbox->setGeometry(20, 200, 200, 25);
  hide_other_checkbox->setTristate(false);
  hide_other_checkbox->setToolTip("Hide annotations with a different label");
  if(filter_params->hide_other)
  {
    hide_other_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    hide_other_checkbox->setCheckState(Qt::Unchecked);
  }

  hide_in_list_checkbox = new QCheckBox("Filter list only ", annot_filter_dialog);
  hide_in_list_checkbox->setGeometry(20, 245, 200, 25);
  hide_in_list_checkbox->setTristate(false);
  hide_in_list_checkbox->setToolTip("Filter affects the annotationlist only, not the annotation markers in the signal window");
  if(filter_params->hide_in_list_only)
  {
    hide_in_list_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    hide_in_list_checkbox->setCheckState(Qt::Unchecked);
  }

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

  QObject::connect(CloseButton,           SIGNAL(clicked()),            annot_filter_dialog, SLOT(close()));
  QObject::connect(ApplyButton,           SIGNAL(clicked()),            this,                SLOT(apply_filter()));
  QObject::connect(t1_dspinbox,           SIGNAL(valueChanged(double)), this,                SLOT(t1_dspinbox_changed(double)));
  QObject::connect(t2_dspinbox,           SIGNAL(valueChanged(double)), this,                SLOT(t2_dspinbox_changed(double)));
  QObject::connect(hide_other_checkbox,   SIGNAL(stateChanged(int)),    this,                SLOT(hide_other_checkbox_changed(int)));
  QObject::connect(hide_in_list_checkbox, SIGNAL(stateChanged(int)),    this,                SLOT(hide_in_list_checkbox_changed(int)));
  QObject::connect(invert_checkbox,       SIGNAL(stateChanged(int)),    this,                SLOT(invert_checkbox_changed(int)));

  annot_filter_dialog->exec();
}


void UI_AnnotFilterWindow::invert_checkbox_changed(int state)
{
  if(state == Qt::Checked)
  {
    filter_params->invert = 1;
  }
  else
  {
    filter_params->invert = 0;
  }
}


void UI_AnnotFilterWindow::hide_other_checkbox_changed(int state)
{
  if(state == Qt::Checked)
  {
    filter_params->hide_other = 1;
  }
  else
  {
    filter_params->hide_other = 0;
  }
}


void UI_AnnotFilterWindow::hide_in_list_checkbox_changed(int state)
{
  if(state == Qt::Checked)
  {
    filter_params->hide_in_list_only = 1;
  }
  else
  {
    filter_params->hide_in_list_only = 0;
  }
}


void UI_AnnotFilterWindow::t1_dspinbox_changed(double t1_val)
{
  double t2_val, diff;

  t2_val = t2_dspinbox->value();

  diff =  t2_val - t1_val;
  if(diff < 0.001)
  {
    t2_val = t1_val + 0.001;

    t2_dspinbox->setValue(t2_val);
  }

  filter_params->tmin = t1_val;
  filter_params->tmax = t2_val;
}


void UI_AnnotFilterWindow::t2_dspinbox_changed(double t2_val)
{
  double t1_val, diff;

  t1_val = t1_dspinbox->value();

  diff =  t2_val - t1_val;
  if(diff < 0.001)
  {
    t1_val = t2_val - 0.001;

    t1_dspinbox->setValue(t1_val);
  }

  filter_params->tmin = t1_val;
  filter_params->tmax = t2_val;
}


void UI_AnnotFilterWindow::apply_filter()
{
  int i, sz, is_set=0;

  long long t1, t_diff, t_min, t_max;

  char annot_str[MAX_ANNOTATION_LEN + 1];

  struct annotationblock *annot, *annot_before;

  t_min = filter_params->tmin * TIME_DIMENSION;

  t_max = filter_params->tmax * TIME_DIMENSION;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strcpy(annot_str, annot->annotation);
    remove_leading_spaces(sel_annot_str);
    remove_trailing_spaces(annot_str);

    if(strcmp(annot_str, sel_annot_str))
    {
      if(filter_params->hide_other)
      {
        annot->hided_in_list = 1;

        if(filter_params->hide_in_list_only)
        {
          annot->hided = 0;
        }
        else
        {
          annot->hided = 1;
        }
      }
      else
      {
        annot->hided_in_list = 0;

        annot->hided = 0;
      }

      continue;
    }

    if(is_set == 0)
    {
      is_set = 1;

      t1 = annot->onset;

      annot->hided_in_list = 1;

      if(filter_params->hide_in_list_only)
      {
        annot->hided = 0;
      }
      else
      {
        annot->hided = 1;
      }

      annot_before = annot;

      continue;
    }

    t_diff = annot->onset - t1;

    t1 = annot->onset;

    if(filter_params->invert)
    {
      if((t_diff < t_min) || (t_diff > t_max))
      {
        annot->hided_in_list = 1;

        if(filter_params->hide_in_list_only)
        {
          annot->hided = 0;
        }
        else
        {
          annot->hided = 1;
        }
      }
      else
      {
        annot->hided_in_list = 0;

        annot->hided = 0;

        annot_before->hided_in_list = 0;

        annot_before->hided = 0;
      }
    }
    else
    {
      if((t_diff >= t_min) && (t_diff <= t_max))
      {
        annot->hided_in_list = 1;

        if(filter_params->hide_in_list_only)
        {
          annot->hided = 0;
        }
        else
        {
          annot->hided = 1;
        }
      }
      else
      {
        annot->hided_in_list = 0;

        annot->hided = 0;

        annot_before->hided_in_list = 0;

        annot_before->hided = 0;
      }
    }

    annot_before = annot;
  }

  annots_dock->updateList();

  mainwindow->maincurve->update();
}












