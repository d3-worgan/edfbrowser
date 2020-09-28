/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2020 Teunis van Beelen
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



#include "show_edf_hdr.h"



UI_EDFhdrwindow::UI_EDFhdrwindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  EDFhdrDialog = new QDialog;
  EDFhdrDialog->setMinimumSize(770, 680);
  EDFhdrDialog->setWindowTitle("EDF header info");
  EDFhdrDialog->setModal(true);
  EDFhdrDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget;
  filelist->setMinimumSize(600, 75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  signallist = new QTableWidget;
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setEditTriggers(QAbstractItemView::NoEditTriggers);

  pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  label1 = new QLabel("Subject code");
  label2 = new QLabel("Sex");
  label3 = new QLabel("Start");
  label4 = new QLabel("Duration");
  label5 = new QLabel("Reserved");
  label6 = new QLabel("Birthdate");
  label7 = new QLabel("Subject name");
  label8 = new QLabel("Additional info");
  label9 = new QLabel("Startdate");
  label10 = new QLabel("Administr. code");
  label11 = new QLabel("Technician");
  label12 = new QLabel("Device");
  label13 = new QLabel("Additional info");
  label20 = new QLabel("Datarecord duration");
  label21 = new QLabel("Version");

  line_edit1 = new QLineEdit;
  line_edit1->setReadOnly(true);
  line_edit2 = new QLineEdit;
  line_edit2->setReadOnly(true);
  line_edit3 = new QLineEdit;
  line_edit3->setReadOnly(true);
  line_edit4 = new QLineEdit;
  line_edit4->setReadOnly(true);
  line_edit5 = new QLineEdit;
  line_edit5->setReadOnly(true);
  line_edit6 = new QLineEdit;
  line_edit6->setReadOnly(true);
  line_edit7 = new QLineEdit;
  line_edit7->setReadOnly(true);
  line_edit8 = new QLineEdit;
  line_edit8->setReadOnly(true);
  line_edit9 = new QLineEdit;
  line_edit9->setReadOnly(true);
  line_edit10 = new QLineEdit;
  line_edit10->setReadOnly(true);
  line_edit11 = new QLineEdit;
  line_edit11->setReadOnly(true);
  line_edit12 = new QLineEdit;
  line_edit12->setReadOnly(true);
  line_edit13 = new QLineEdit;
  line_edit13->setReadOnly(true);
  line_edit20 = new QLineEdit;
  line_edit20->setReadOnly(true);
  line_edit21 = new QLineEdit;
  line_edit21->setReadOnly(true);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(label1);
  hlayout2->addWidget(line_edit1);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(label2);
  hlayout3->addWidget(line_edit2);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(label3);
  hlayout4->addWidget(line_edit3);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addWidget(label4);
  hlayout5->addWidget(line_edit4);

  QHBoxLayout *hlayout6 = new QHBoxLayout;
  hlayout6->addWidget(label5);
  hlayout6->addWidget(line_edit5);

  QHBoxLayout *hlayout7 = new QHBoxLayout;
  hlayout7->addWidget(label6);
  hlayout7->addWidget(line_edit6);

  QHBoxLayout *hlayout8 = new QHBoxLayout;
  hlayout8->addWidget(label7);
  hlayout8->addWidget(line_edit7);

  QHBoxLayout *hlayout9 = new QHBoxLayout;
  hlayout9->addWidget(label8);
  hlayout9->addWidget(line_edit8);

  QHBoxLayout *hlayout10 = new QHBoxLayout;
  hlayout10->addWidget(label9);
  hlayout10->addWidget(line_edit9);

  QHBoxLayout *hlayout11 = new QHBoxLayout;
  hlayout11->addWidget(label10);
  hlayout11->addWidget(line_edit10);

  QHBoxLayout *hlayout12 = new QHBoxLayout;
  hlayout12->addWidget(label11);
  hlayout12->addWidget(line_edit11);

  QHBoxLayout *hlayout13 = new QHBoxLayout;
  hlayout13->addWidget(label12);
  hlayout13->addWidget(line_edit12);

  QHBoxLayout *hlayout14 = new QHBoxLayout;
  hlayout14->addWidget(label13);
  hlayout14->addWidget(line_edit13);

  QHBoxLayout *hlayout15 = new QHBoxLayout;
  hlayout15->addWidget(label20);
  hlayout15->addWidget(line_edit20);

  QHBoxLayout *hlayout16 = new QHBoxLayout;
  hlayout16->addWidget(label21);
  hlayout16->addWidget(line_edit21);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addLayout(hlayout2);
  vlayout2->addLayout(hlayout3);
  vlayout2->addLayout(hlayout4);
  vlayout2->addLayout(hlayout5);
  vlayout2->addLayout(hlayout6);
  vlayout2->addLayout(hlayout7);
  vlayout2->addLayout(hlayout8);
  vlayout2->addLayout(hlayout9);
  vlayout2->addLayout(hlayout10);
  vlayout2->addLayout(hlayout11);
  vlayout2->addLayout(hlayout12);
  vlayout2->addLayout(hlayout13);
  vlayout2->addLayout(hlayout14);
  vlayout2->addLayout(hlayout15);
  vlayout2->addLayout(hlayout16);
  vlayout2->addStretch(1000);

  QTabWidget *tabholder = new QTabWidget;

  tab1 = new QWidget;

  tabholder->addTab(tab1, "General");
  tabholder->addTab(signallist, "Signals");

  tab1->setLayout(vlayout2);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist);
  vlayout1->addSpacing(20);
  vlayout1->addWidget(tabholder, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  EDFhdrDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()),              EDFhdrDialog, SLOT(close()));
  QObject::connect(filelist,    SIGNAL(currentRowChanged(int)), this,         SLOT(show_params(int)));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  EDFhdrDialog->exec();
}


void UI_EDFhdrwindow::show_params(int row)
{
  int i,
      signal_cnt;

  char str[512];

  long long file_duration;

  struct date_time_struct date_time;


  if(row<0)  return;

  if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
  {
    label1->setText("Subject code");
    label2->setText("Sex");

    label6->setVisible(true);
    label7->setVisible(true);
    label8->setVisible(true);
    label9->setVisible(true);
    label10->setVisible(true);
    label11->setVisible(true);
    label12->setVisible(true);
    label13->setVisible(true);

    line_edit6->setVisible(true);
    line_edit7->setVisible(true);
    line_edit8->setVisible(true);
    line_edit9->setVisible(true);
    line_edit10->setVisible(true);
    line_edit11->setVisible(true);
    line_edit12->setVisible(true);
    line_edit13->setVisible(true);

    line_edit1->setText(mainwindow->edfheaderlist[row]->plus_patientcode);
    line_edit2->setText(mainwindow->edfheaderlist[row]->plus_gender);
    line_edit6->setText(mainwindow->edfheaderlist[row]->plus_birthdate);
    line_edit7->setText(mainwindow->edfheaderlist[row]->plus_patient_name);
    line_edit8->setText(mainwindow->edfheaderlist[row]->plus_patient_additional);
    line_edit9->setText(mainwindow->edfheaderlist[row]->plus_startdate);
    line_edit10->setText(mainwindow->edfheaderlist[row]->plus_admincode);
    line_edit11->setText(mainwindow->edfheaderlist[row]->plus_technician);
    line_edit12->setText(mainwindow->edfheaderlist[row]->plus_equipment);
    line_edit13->setText(mainwindow->edfheaderlist[row]->plus_recording_additional);
  }
  else // old EDF
  {
    label1->setText("Subject");
    label2->setText("Recording");

    label6->setVisible(false);
    label7->setVisible(false);
    label8->setVisible(false);
    label9->setVisible(false);
    label10->setVisible(false);
    label11->setVisible(false);
    label12->setVisible(false);
    label13->setVisible(false);

    line_edit6->setVisible(false);
    line_edit7->setVisible(false);
    line_edit8->setVisible(false);
    line_edit9->setVisible(false);
    line_edit10->setVisible(false);
    line_edit11->setVisible(false);
    line_edit12->setVisible(false);
    line_edit13->setVisible(false);

    line_edit1->setText(mainwindow->edfheaderlist[row]->patient);
    line_edit2->setText(mainwindow->edfheaderlist[row]->recording);
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str, 400, "%i %s %i  %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);

  if(mainwindow->edfheaderlist[row]->starttime_offset != 0LL)
  {
#ifdef Q_OS_WIN32
    __mingw_snprintf(str + strlen(str), 100, ".%07lli", mainwindow->edfheaderlist[row]->starttime_offset);
#else
    snprintf(str + strlen(str), 100, ".%07lli", mainwindow->edfheaderlist[row]->starttime_offset);
#endif

    remove_trailing_zeros(str);
  }

  line_edit3->setText(str);

  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;

  if((file_duration / TIME_DIMENSION) / 10)
  {
    snprintf(str, 512,
            "%2i:%02i:%02i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL));
  }
  else
  {
    snprintf(str, 512,
            "%2i:%02i:%02i.%06i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL),
            (int)((file_duration % TIME_DIMENSION) / 10LL));
  }

  line_edit4->setText(str);

  line_edit5->setText(mainwindow->edfheaderlist[row]->reserved);

  snprintf(str,  512, "%.12f", mainwindow->edfheaderlist[row]->data_record_duration);

  remove_trailing_zeros(str);

  line_edit20->setText(str);

  line_edit21->setText(mainwindow->edfheaderlist[row]->version);

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  signallist->setColumnCount(10);
  signallist->setRowCount(signal_cnt);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Samplefrequency";
  horizontallabels += "Physical maximum";
  horizontallabels += "Physical minimum";
  horizontallabels += "Physical dimension";
  horizontallabels += "Digital maximum";
  horizontallabels += "Digital minimum";
  horizontallabels += "Samples per record";
  horizontallabels += "Prefilter";
  horizontallabels += "Transducer";
  signallist->setHorizontalHeaderLabels(horizontallabels);

  QLabel *ql;

  int margin=3;

  for(i=0; i<signal_cnt; i++)
  {
    signallist->setRowHeight(i, 25);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].label);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 0, ql);
    convert_to_metric_suffix(str, mainwindow->edfheaderlist[row]->edfparam[i].sf_f, 3, 512);
    strlcat(str, "Hz", 512);
    remove_trailing_zeros(str);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 1, ql);
    snprintf(str, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_max);
    remove_trailing_zeros(str);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 2, ql);
    snprintf(str, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_min);
    remove_trailing_zeros(str);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 3, ql);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].physdimension);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 4, ql);
    snprintf(str, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_max);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 5, ql);
    snprintf(str, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_min);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 6, ql);
    snprintf(str, 512, "%i", mainwindow->edfheaderlist[row]->edfparam[i].smp_per_record);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 7, ql);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].prefilter);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 8, ql);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].transducer);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 9, ql);
  }

  signallist->resizeColumnsToContents();
}

















