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



#include "annotations_dock.h"



UI_AnnotationEditwindow::UI_AnnotationEditwindow(struct edfhdrblock *e_hdr, QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  edf_hdr = e_hdr;

  is_deleted = 0;

  dockedit = new QToolBar("Annotation editor", w_parent);
  dockedit->setOrientation(Qt::Horizontal);
  dockedit->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  dockedit->setAttribute(Qt::WA_DeleteOnClose, true);

  annot_edit_frame = new QFrame;
  annot_edit_frame->setMinimumSize(1050, 45);

  dockedit->addWidget(annot_edit_frame);

  descriptionLabel = new QLabel(annot_edit_frame);
  descriptionLabel->setGeometry(10, 10, 78, 25);
  descriptionLabel->setText("Description");

  annot_descript_lineEdit = new QLineEdit(annot_edit_frame);
  annot_descript_lineEdit->setGeometry(90, 10, 150, 25);

  completer = new QCompleter(annot_edit_frame);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  annot_descript_lineEdit->setCompleter(completer);

  onsetLabel = new QLabel(annot_edit_frame);
  onsetLabel->setGeometry(250, 10, 48, 25);
  onsetLabel->setText("Onset");

  posNegTimebox = new QComboBox(annot_edit_frame);
  posNegTimebox->setGeometry(300, 10, 35, 25);
  posNegTimebox->setEditable(false);
  posNegTimebox->addItem("+");
  posNegTimebox->addItem("-");

  onset_daySpinbox = new QSpinBox(annot_edit_frame);
  onset_daySpinbox->setGeometry(335, 10, 45, 25);
  onset_daySpinbox->setRange(0, 99);
  onset_daySpinbox->setSingleStep(1);
  onset_daySpinbox->setValue(0);
  onset_daySpinbox->setToolTip("24-hour units relative to starttime");

  onset_timeEdit = new QTimeEdit(annot_edit_frame);
  onset_timeEdit->setGeometry(380, 10, 100, 25);
  onset_timeEdit->setDisplayFormat("hh:mm:ss.zzz");
  onset_timeEdit->setMinimumTime(QTime(-1, 0, 0, 0));
  onset_timeEdit->setToolTip("Onset time of the event (hh:mm:ss:mmm) relative to starttime");

  durationLabel = new QLabel(annot_edit_frame);
  durationLabel->setGeometry(490, 10, 58, 25);
  durationLabel->setText("Duration");

  duration_spinbox = new QDoubleSpinBox(annot_edit_frame);
  duration_spinbox->setGeometry(550, 10, 120, 25);
  duration_spinbox->setRange(-1.0, 10000.0);
  duration_spinbox->setSingleStep(1.0);
  duration_spinbox->setDecimals(3);
  duration_spinbox->setSuffix(" sec");
  duration_spinbox->setValue(-1.0);
  duration_spinbox->setToolTip("Duration of the event (hh:mm:ss:mmm), -1 if not applicable)");

  modifybutton = new QPushButton(annot_edit_frame);
  modifybutton->setGeometry(700, 10, 100, 25);
  modifybutton->setText("Modify");
  modifybutton->setEnabled(false);

  deletebutton = new QPushButton(annot_edit_frame);
  deletebutton->setGeometry(820, 10, 100, 25);
  deletebutton->setText("Delete");
  deletebutton->setShortcut(QKeySequence::Delete);
  deletebutton->setEnabled(false);

  createbutton = new QPushButton(annot_edit_frame);
  createbutton->setGeometry(940, 10, 100, 25);
  createbutton->setText("Create");

  mainwindow->annot_editor_active = 1;

  mainwindow->show_annot_markers = 1;

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  update_description_completer();

  QObject::connect(modifybutton, SIGNAL(clicked()),               this, SLOT(modifyButtonClicked()));
  QObject::connect(deletebutton, SIGNAL(clicked()),               this, SLOT(deleteButtonClicked()));
  QObject::connect(createbutton, SIGNAL(clicked()),               this, SLOT(createButtonClicked()));
  QObject::connect(dockedit,     SIGNAL(destroyed(QObject *)),    this, SLOT(dockedit_destroyed(QObject *)));
}


UI_AnnotationEditwindow::~UI_AnnotationEditwindow()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeToolBar(dockedit);

    mainwindow->annot_editor_active = 0;

    mainwindow->annotationEditDock = NULL;
  }
}


void UI_AnnotationEditwindow::dockedit_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->annot_editor_active = 0;

    mainwindow->annotationEditDock = NULL;

    delete this;
  }
}


void UI_AnnotationEditwindow::modifyButtonClicked()
{
  int file_num;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot = edfplus_annotation_get_item(annot_list, annot_num);

  annot->onset = annotEditGetOnset();

  if(posNegTimebox->currentIndex() == 1)
  {
    annot->onset = -(annot->onset);
  }

  annot->onset += edf_hdr->starttime_offset;

  if(dblcmp(duration_spinbox->value(), 0.0) > 0)
  {
    snprintf(annot->duration, 16, "%f", duration_spinbox->value());

    annot->long_duration = edfplus_annotation_get_long_from_number(annot->duration);
  }
  else
  {
    annot->duration[0] = 0;

    annot->long_duration = 0LL;
  }

  strncpy(annot->description, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

  annot->description[MAX_ANNOTATION_LEN] = 0;

  annot->modified = 1;

  annot->selected = 1;

  mainwindow->annotations_edited = 1;

  file_num = mainwindow->get_filenum((struct edfhdrblock *)(annot->edfhdr));
  if(file_num >= 0)
  {
    mainwindow->annotations_dock[file_num]->updateList();
  }

  update_description_completer();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::deleteButtonClicked()
{
  int sz, file_num;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot = edfplus_annotation_get_item(annot_list, annot_num);

  edfplus_annotation_remove_item(annot_list, annot_num);

  sz = edfplus_annotation_size(annot_list);

  if(annot_num < sz)
  {
    annot->selected = 1;

    annot->jump = 1;
  }

  modifybutton->setEnabled(false);

  deletebutton->setEnabled(false);

  mainwindow->annotations_edited = 1;

  mainwindow->save_act->setEnabled(true);

  file_num = mainwindow->get_filenum((struct edfhdrblock *)(annot->edfhdr));
  if(file_num >= 0)
  {
    mainwindow->annotations_dock[file_num]->updateList();
  }

  update_description_completer();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::createButtonClicked()
{
  int file_num;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock annotation;

  memset(&annotation, 0, sizeof(struct annotationblock));

  annotation.onset = annotEditGetOnset();

  if(posNegTimebox->currentIndex() == 1)
  {
    annotation.onset = -(annotation.onset);
  }

  annotation.onset += edf_hdr->starttime_offset;

  annotation.edfhdr = edf_hdr;

  if(dblcmp(duration_spinbox->value(), 0.0) > 0)
  {
    snprintf(annotation.duration, 16, "%f", duration_spinbox->value());

    annotation.long_duration = edfplus_annotation_get_long_from_number(annotation.duration);
  }
  else
  {
    annotation.duration[0] = 0;

    annotation.long_duration = 0LL;
  }

  strncpy(annotation.description, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

  annotation.description[MAX_ANNOTATION_LEN] = 0;

  annotation.modified = 1;

  edfplus_annotation_add_item(annot_list, annotation);

  mainwindow->annotations_edited = 1;

  file_num = mainwindow->get_filenum((struct edfhdrblock *)(annotation.edfhdr));
  if(file_num >= 0)
  {
    mainwindow->annotations_dock[file_num]->updateList();
  }

  update_description_completer();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::annotEditSetOnset(long long onset)
{
  QTime ta;

  if(onset < 0LL)
  {
    onset_daySpinbox->setValue((-(onset)) / TIME_DIMENSION / 86400);

    ta.setHMS(((-(onset)) / TIME_DIMENSION / 3600) % 24,
            (((-(onset)) / TIME_DIMENSION) % 3600) / 60,
            ((-(onset)) / TIME_DIMENSION) % 60,
            ((-(onset)) % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(1);
  }
  else
  {
    onset_daySpinbox->setValue(onset / TIME_DIMENSION / 86400);

    ta.setHMS((onset / TIME_DIMENSION / 3600) % 24,
            ((onset / TIME_DIMENSION) % 3600) / 60,
            (onset / TIME_DIMENSION) % 60,
            (onset % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(0);
  }

  onset_timeEdit->setTime(ta);
}



long long UI_AnnotationEditwindow::annotEditGetOnset(void)
{
  long long tmp;

  tmp = onset_daySpinbox->value() * 86400;
  tmp += onset_timeEdit->time().hour() * 3600;
  tmp += onset_timeEdit->time().minute() * 60;
  tmp += onset_timeEdit->time().second();
  tmp *= TIME_DIMENSION;
  tmp += (onset_timeEdit->time().msec() * (TIME_DIMENSION / 1000));

  if(posNegTimebox->currentIndex() == 1)
  {
    tmp *= -1LL;
  }

  return tmp;
}



void UI_AnnotationEditwindow::annotEditSetDuration(long long duration)
{
  duration_spinbox->setValue(((double)duration) / TIME_DIMENSION);
}


void UI_AnnotationEditwindow::set_edf_header(struct edfhdrblock *e_hdr)
{
  edf_hdr = e_hdr;
}


void UI_AnnotationEditwindow::set_selected_annotation(int annot_nr)
{
  long long l_tmp;

  QTime ta;

  annot_num = annot_nr;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot = edfplus_annotation_get_item(annot_list, annot_num);

  annot_descript_lineEdit->setText(QString::fromUtf8(annot->description));

  l_tmp = annot->onset - ((struct edfhdrblock *)(annot->edfhdr))->starttime_offset;

  if(l_tmp < 0LL)
  {
    onset_daySpinbox->setValue((-(l_tmp)) / TIME_DIMENSION / 86400);

    ta.setHMS(((-(l_tmp)) / TIME_DIMENSION / 3600) % 24,
            (((-(l_tmp)) / TIME_DIMENSION) % 3600) / 60,
            ((-(l_tmp)) / TIME_DIMENSION) % 60,
            ((-(l_tmp)) % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(1);
  }
  else
  {
    onset_daySpinbox->setValue(l_tmp / TIME_DIMENSION / 86400);

    ta.setHMS((l_tmp / TIME_DIMENSION / 3600) % 24,
            ((l_tmp / TIME_DIMENSION) % 3600) / 60,
            (l_tmp / TIME_DIMENSION) % 60,
            (l_tmp % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(0);
  }
  onset_timeEdit->setTime(ta);

  if(strlen(annot->duration))
  {
    duration_spinbox->setValue(atof(annot->duration));
  }
  else
  {
    duration_spinbox->setValue(-1);
  }

  modifybutton->setEnabled(true);

  deletebutton->setEnabled(true);
}


void UI_AnnotationEditwindow::set_selected_annotation(struct annotationblock *annot)
{
  int n;

  long long l_tmp;

  QTime ta;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  n = edfplus_annotation_get_index(annot_list, annot);

  if(n < 0)  return;

  annot_num = n;

  annot_descript_lineEdit->setText(QString::fromUtf8(annot->description));

  l_tmp = annot->onset - ((struct edfhdrblock *)(annot->edfhdr))->starttime_offset;

  if(l_tmp < 0LL)
  {
    onset_daySpinbox->setValue((-(l_tmp)) / TIME_DIMENSION / 86400);

    ta.setHMS(((-(l_tmp)) / TIME_DIMENSION / 3600) % 24,
            (((-(l_tmp)) / TIME_DIMENSION) % 3600) / 60,
            ((-(l_tmp)) / TIME_DIMENSION) % 60,
            ((-(l_tmp)) % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(1);
  }
  else
  {
    onset_daySpinbox->setValue(l_tmp / TIME_DIMENSION / 86400);

    ta.setHMS((l_tmp / TIME_DIMENSION / 3600) % 24,
            ((l_tmp / TIME_DIMENSION) % 3600) / 60,
            (l_tmp / TIME_DIMENSION) % 60,
            (l_tmp % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(0);
  }
  onset_timeEdit->setTime(ta);

  if(strlen(annot->duration))
  {
    duration_spinbox->setValue(atof(annot->duration));
  }
  else
  {
    duration_spinbox->setValue(-1);
  }

  modifybutton->setEnabled(false);

  deletebutton->setEnabled(false);
}


void UI_AnnotationEditwindow::update_description_completer(void)
{
  int i;

  QStringList string_list;

  QStringListModel *model;

  mainwindow->get_unique_annotations(edf_hdr);

  for(i=0; i<MAX_UNIQUE_ANNOTATIONS; i++)
  {
    if(edf_hdr->unique_annotations_list[i][0] == 0)  break;

    string_list << edf_hdr->unique_annotations_list[i];
  }

  model = (QStringListModel *)(completer->model());
  if(model == NULL)
  {
    model = new QStringListModel(this);
  }

  model->setStringList(string_list);

  completer->setModel(model);
}



















