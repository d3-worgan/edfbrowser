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



#include "export_filtered_signals.h"



UI_ExportFilteredSignalsWindow::UI_ExportFilteredSignalsWindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(715, 578);
  myobjectDialog->setMaximumSize(715, 578);
  myobjectDialog->setWindowTitle("Export Filtered Signals");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  tree = new QTreeView(myobjectDialog);
  tree->setGeometry(20, 66, 405, 432);
  tree->setHeaderHidden(true);
  tree->setIndentation(30);
  tree->setSelectionMode(QAbstractItemView::NoSelection);
  tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree->setSortingEnabled(false);
  tree->setDragDropMode(QAbstractItemView::NoDragDrop);

  t_model = new QStandardItemModel(this);

  label1 = new QLabel(myobjectDialog);
  label1->setGeometry(20, 20, 685, 25);

  label2 = new QLabel(myobjectDialog);
  label2->setGeometry(485, 359, 100, 25);
  label2->setText("from datarecord");
  label2->setEnabled(false);

  label3 = new QLabel(myobjectDialog);
  label3->setGeometry(485, 424, 100, 25);
  label3->setText("to datarecord");
  label3->setEnabled(false);

  label4 = new QLabel(myobjectDialog);
  label4->setGeometry(605, 382, 100, 25);
  label4->setEnabled(false);

  label5 = new QLabel(myobjectDialog);
  label5->setGeometry(605, 447, 100, 25);
  label5->setEnabled(false);

  radioButton1 = new QRadioButton("whole duration", myobjectDialog);
  radioButton1->setGeometry(485, 299, 120, 25);
  radioButton1->setChecked(true);
  radioButton1->setEnabled(false);

  radioButton2 = new QRadioButton("selection", myobjectDialog);
  radioButton2->setGeometry(485, 324, 100, 25);
  radioButton2->setEnabled(false);

  spinBox1 = new QSpinBox(myobjectDialog);
  spinBox1->setGeometry(485, 384, 100, 25);
  spinBox1->setRange(1, 2147483647);
  spinBox1->setValue(1);
  spinBox1->setEnabled(false);

  spinBox2 = new QSpinBox(myobjectDialog);
  spinBox2->setGeometry(485, 449, 100, 25);
  spinBox2->setRange(1, 2147483647);
  spinBox2->setValue(2147483647);
  spinBox2->setEnabled(false);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 528, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(575, 528, 100, 25);
  pushButton2->setText("Close");

  pushButton3 = new QPushButton(myobjectDialog);
  pushButton3->setGeometry(200, 528, 100, 25);
  pushButton3->setText("Export");
  pushButton3->setEnabled(false);

  QObject::connect(pushButton1,  SIGNAL(clicked()),         this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2,  SIGNAL(clicked()),         myobjectDialog, SLOT(close()));
  QObject::connect(pushButton3,  SIGNAL(clicked()),         this,           SLOT(StartExport()));
  QObject::connect(spinBox1,     SIGNAL(valueChanged(int)), this,           SLOT(spinBox1changed(int)));
  QObject::connect(spinBox2,     SIGNAL(valueChanged(int)), this,           SLOT(spinBox2changed(int)));
  QObject::connect(radioButton1, SIGNAL(toggled(bool)),     this,           SLOT(radioButton1Toggled(bool)));
  QObject::connect(radioButton2, SIGNAL(toggled(bool)),     this,           SLOT(radioButton2Toggled(bool)));

  edfhdr = NULL;
  inputfile = NULL;
  outputfile = NULL;
  file_num = -1;

  inputpath[0] = 0;

  myobjectDialog->exec();
}


void UI_ExportFilteredSignalsWindow::spinBox1changed(int value)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad[256];

  QObject::disconnect(spinBox2, SIGNAL(valueChanged(int)), this, SLOT(spinBox2changed(int)));
  spinBox2->setMinimum(value);
  QObject::connect(spinBox2,    SIGNAL(valueChanged(int)), this, SLOT(spinBox2changed(int)));

  if(edfhdr == NULL)
  {
    return;
  }
  days = (int)((((value - 1) * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
  seconds = ((value - 1) * edfhdr->long_data_record_duration) / TIME_DIMENSION;
  if(seconds < 0)
  {
    seconds = 0;
  }
  seconds %= 86400LL;
  milliSec = ((value - 1) * edfhdr->long_data_record_duration) % TIME_DIMENSION;
  milliSec /= 10000LL;
  if(days)
  {
    sprintf(scratchpad, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    sprintf(scratchpad, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label4->setText(scratchpad);
}


void UI_ExportFilteredSignalsWindow::spinBox2changed(int value)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad[256];

  QObject::disconnect(spinBox1, SIGNAL(valueChanged(int)), this, SLOT(spinBox1changed(int)));
  spinBox1->setMaximum(value);
  QObject::connect(spinBox1,    SIGNAL(valueChanged(int)), this, SLOT(spinBox1changed(int)));

  if(edfhdr == NULL)
  {
    return;
  }
  days = (int)(((value * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
  seconds = (value * edfhdr->long_data_record_duration) / TIME_DIMENSION;
  seconds %= 86400LL;
  milliSec = (value * edfhdr->long_data_record_duration) % TIME_DIMENSION;
  milliSec /= 10000LL;
  if(days)
  {
    sprintf(scratchpad, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    sprintf(scratchpad, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label5->setText(scratchpad);
}


void UI_ExportFilteredSignalsWindow::radioButton1Toggled(bool checked)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad[256];

  if(checked == true)
  {
    spinBox1->setEnabled(false);
    spinBox2->setEnabled(false);
    label2->setEnabled(false);
    label3->setEnabled(false);
    label4->setEnabled(false);
    label5->setEnabled(false);

    if(edfhdr == NULL)
    {
      return;
    }
    spinBox1->setValue(1);
    spinBox2->setMaximum(edfhdr->datarecords);
    spinBox2->setValue(edfhdr->datarecords);
    spinBox1->setMaximum(edfhdr->datarecords);

    days = (int)(((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
    seconds = (edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION;
    seconds %= 86400LL;
    milliSec = (edfhdr->datarecords * edfhdr->long_data_record_duration) % TIME_DIMENSION;
    milliSec /= 10000LL;

    if(days > 0)
    {
      label4->setText("0d 0:00:00.000");

      sprintf(scratchpad, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
    }
    else
    {
      label4->setText("0:00:00.000");

      sprintf(scratchpad, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
    }

    label5->setText(scratchpad);
  }
}


void UI_ExportFilteredSignalsWindow::radioButton2Toggled(bool checked)
{
  if(checked == true)
  {
    spinBox1->setEnabled(true);
    spinBox2->setEnabled(true);
    label2->setEnabled(true);
    label3->setEnabled(true);
    label4->setEnabled(true);
    label5->setEnabled(true);
  }
}


void UI_ExportFilteredSignalsWindow::SelectFileButton()
{
  int i, j,
      days;

  long long seconds,
            milliSec;

  char txt_string[2048],
       str[256];

  label1->clear();
  label4->clear();
  label5->clear();

  inputfile = NULL;
  outputfile = NULL;

  inputpath[0] = 0;

  edfhdr = NULL;

  file_num = -1;

  pushButton3->setEnabled(false);
  spinBox1->setEnabled(false);
  spinBox2->setEnabled(false);
  radioButton1->setChecked(true);
  radioButton1->setEnabled(false);
  radioButton2->setEnabled(false);
  label2->setEnabled(false);
  label3->setEnabled(false);
  label4->setEnabled(false);
  label5->setEnabled(false);

  UI_activeFileChooserWindow afchooser(&file_num, mainwindow);

  if(file_num < 0)
  {
    return;
  }

  edfhdr = mainwindow->edfheaderlist[file_num];

  strcpy(inputpath, edfhdr->filename);

  inputfile = edfhdr->file_hdl;
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.", inputpath);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(txt_string));
    messagewindow.exec();

    inputpath[0] = 0;

    edfhdr = NULL;

    file_num = -1;

    return;
  }

  if(edfhdr->datarecords > 2147483647LL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Failure", "This file contains more than 2147483647 datarecords.\n"
                                                                "This tool can not handle more than 2147483647 datarecords.");
    messagewindow.exec();

    inputfile = NULL;

    inputpath[0] = 0;

    edfhdr = NULL;

    file_num = -1;

    return;
  }

/***************** load signalproperties ******************************/

  label1->setText(inputpath);

  pushButton3->setEnabled(true);

  spinBox1->setValue(1);
  spinBox2->setMaximum(edfhdr->datarecords);
  spinBox2->setValue(edfhdr->datarecords);
  spinBox1->setMaximum(edfhdr->datarecords);

  radioButton1->setEnabled(true);
  radioButton2->setEnabled(true);

  label4->setText("0:00:00.000");
  days = (int)(((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
  seconds = (edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION;
  seconds %= 86400LL;
  milliSec = (edfhdr->datarecords * edfhdr->long_data_record_duration) % TIME_DIMENSION;
  milliSec /= 10000LL;
  if(days)
  {
    sprintf(txt_string, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    sprintf(txt_string, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label5->setText(txt_string);

  populate_tree_view();
}


void UI_ExportFilteredSignalsWindow::StartExport()
{
  int i, j, k, n,
      new_edfsignals,
      datarecords=0,
      annot_smp_per_record,
      annot_recordsize,
      timestamp_digits=0,
      timestamp_decimals=0,
      annot_len,
      annot_cnt=0,
      tallen=0,
      len,
      annots_per_datrec=0,
      smplrt,
      tmp,
      val,
      progress_steps,
      datrecs_processed,
      annot_list_sz=0,
      smp_per_record[MAXSIGNALS];

  char *readbuf=NULL,
       scratchpad[256];

  double *filtered_blockread_buf[MAXSIGNALS];

  long long new_starttime,
            time_diff,
            onset_diff,
            taltime,
            l_temp,
            endtime=0,
            l_tmp;

  struct date_time_struct dts;

  struct annotation_list new_annot_list,
                         *annot_list=NULL;

  struct annotationblock *annot_ptr=NULL;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  FilteredBlockReadClass *block_reader[MAXSIGNALS];

  for(i=0; i<MAXSIGNALS; i++)
  {
    block_reader[i] = NULL;
  }

  memset(&new_annot_list, 0, sizeof(struct annotation_list));

  QProgressDialog progress("Processing file...", "Abort", 0, 1);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  pushButton3->setEnabled(false);
  spinBox1->setEnabled(false);
  spinBox2->setEnabled(false);
  radioButton1->setEnabled(false);
  radioButton2->setEnabled(false);
  label2->setEnabled(false);
  label3->setEnabled(false);

  if(edfhdr==NULL)
  {
    return;
  }

  if(file_num < 0)
  {
    return;
  }

  new_edfsignals = 0;

  annot_smp_per_record = 0;

  time_diff = (long long)(spinBox1->value() - 1) * edfhdr->long_data_record_duration;

  taltime = (time_diff + edfhdr->starttime_offset) % TIME_DIMENSION;

  endtime = (long long)(spinBox2->value() - (spinBox1->value() - 1)) * edfhdr->long_data_record_duration + taltime;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->filenum != file_num)  continue;

    block_reader[new_edfsignals] = new FilteredBlockReadClass;

    filtered_blockread_buf[new_edfsignals] = block_reader[new_edfsignals]->init_signalcomp(mainwindow->signalcomp[i], 1, 0);

    smp_per_record[new_edfsignals] = block_reader[new_edfsignals]->samples_in_buf();

    new_edfsignals++;
  }

  if(!new_edfsignals)
  {
    showpopupmessage("Error", "No signals present on screen for selected file.");
    goto END_1;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      signalslist[new_edfsignals++] = i;
    }
  }

  datarecords = spinBox2->value() - spinBox1->value() + 1;

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    timestamp_decimals = edfplus_annotation_get_tal_timestamp_decimal_cnt(edfhdr);
    if(timestamp_decimals < 0)
    {
      showpopupmessage("Error", "Internal error, get_tal_timestamp_decimal_cnt()");
      goto END_1;
    }

    timestamp_digits = edfplus_annotation_get_tal_timestamp_digit_cnt(edfhdr);
    if(timestamp_digits < 0)
    {
      showpopupmessage("Error", "Internal error, get_tal_timestamp_digit_cnt()");
      goto END_1;
    }

    annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

    annot_list_sz = edfplus_annotation_size(annot_list);

    for(i=0; i<annot_list_sz; i++)
    {
      annot_ptr = edfplus_annotation_get_item(annot_list, i);

      l_temp = annot_ptr->onset - time_diff;

      if((l_temp >= 0LL) && (l_temp <= endtime))
      {
        edfplus_annotation_add_item(&new_annot_list, *(edfplus_annotation_get_item(annot_list, i)));
      }
    }

    new_starttime = edfhdr->utc_starttime + ((time_diff + edfhdr->starttime_offset) / TIME_DIMENSION);

    onset_diff = (new_starttime - edfhdr->utc_starttime) * TIME_DIMENSION;

    annot_list_sz = edfplus_annotation_size(&new_annot_list);

    if(annot_list_sz > 0)
    {
      for(i=0; i<annot_list_sz; i++)
      {
        annot_ptr = edfplus_annotation_get_item(&new_annot_list, i);

        annot_ptr->onset -= onset_diff;
      }

      edfplus_annotation_sort(&new_annot_list, NULL);

      annots_per_datrec = annot_list_sz / datarecords;

      if(annot_list_sz % datarecords)
      {
        annots_per_datrec++;
      }
    }
    else
    {
      annots_per_datrec = 0;
    }

    annot_len = edfplus_annotation_get_max_annotation_strlen(&new_annot_list);

    annot_recordsize = (annot_len * annots_per_datrec) + timestamp_digits + timestamp_decimals + 4;

    if(timestamp_decimals)
    {
      annot_recordsize++;
    }

    if(edfhdr->edf)
    {
      annot_smp_per_record = annot_recordsize / 2;

      if(annot_recordsize % annot_smp_per_record)
      {
        annot_smp_per_record++;

        annot_recordsize = annot_smp_per_record * 2;
      }
    }
    else
    {
      annot_smp_per_record = annot_recordsize / 3;

      if(annot_recordsize % annot_smp_per_record)
      {
        annot_smp_per_record++;

        annot_recordsize = annot_smp_per_record * 3;
      }
    }
  }
  else
  {
    annot_smp_per_record = 0;

    annot_recordsize = 0;
  }

  readbuf = (char *)malloc(edfhdr->recordsize);
  if(readbuf==NULL)
  {
    showpopupmessage("Error", "Malloc error, (readbuf).");
    goto END_2;
  }
///////////////////////////////////////////////////////////////////

  outputpath[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(outputpath, recent_savedir);
    strcat(outputpath, "/");
  }
  len = strlen(outputpath);
  get_filename_from_path(outputpath + len, inputpath, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(outputpath);
  if(edfhdr->edf)
  {
    strcat(outputpath, "_filtered.edf");

    strcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "EDF files (*.edf *.EDF)").toLocal8Bit().data());
  }
  else
  {
    strcat(outputpath, "_filtered.bdf");

    strcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());
  }

  if(!strcmp(outputpath, ""))
  {
    goto END_3;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    showpopupmessage("Error", "Selected file is in use.");
    goto END_3;
  }

  outputfile = fopeno(outputpath, "wb");
  if(outputfile==NULL)
  {
    showpopupmessage("Error", "Can not open outputfile for writing.");
    goto END_3;
  }

  new_starttime = edfhdr->utc_starttime + ((time_diff + edfhdr->starttime_offset) / TIME_DIMENSION);

  utc_to_date_time(new_starttime, &dts);

  rewind(inputfile);
  if(fread(scratchpad, 168, 1, inputfile)!=1)
  {
    showpopupmessage("Error", "Read error (1).");
    goto END_4;
  }

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    if(scratchpad[98] != 'X')
    {
      sprintf(scratchpad + 98, "%02i-%s-%04i", dts.day, dts.month_str, dts.year);

      scratchpad[109] = ' ';
    }
  }

  if(fwrite(scratchpad, 168, 1, outputfile)!=1)
  {
    showpopupmessage("Error", "Write error (1).");
    goto END_4;
  }

  fprintf(outputfile, "%02i.%02i.%02i%02i.%02i.%02i",
          dts.day,
          dts.month,
          dts.year % 100,
          dts.hour,
          dts.minute,
          dts.second);

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-8i", new_edfsignals * 256 + 512);
  }
  else
  {
    fprintf(outputfile, "%-8i", new_edfsignals * 256 + 256);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF+C");
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "BDF+C");
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  if((!edfhdr->edfplus) && (!edfhdr->bdfplus))
  {
    for(i=0; i<44; i++)
    {
      fputc(' ', outputfile);
    }
  }
  fprintf(outputfile, "%-8i", datarecords);
  snprintf(scratchpad, 256, "%f", edfhdr->data_record_duration);
  convert_trailing_zeros_to_spaces(scratchpad);
  if(scratchpad[7]=='.')
  {
    scratchpad[7] = ' ';
  }
  scratchpad[8] = 0;

  fprintf(outputfile, "%s", scratchpad);
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-4i", new_edfsignals + 1);
  }
  else
  {
    fprintf(outputfile, "%-4i", new_edfsignals);
  }

  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].label);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF Annotations ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "BDF Annotations ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].transducer);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<80; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].physdimension);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<8; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    snprintf(scratchpad, 256, "%f", edfhdr->edfparam[signalslist[i]].phys_min);
    convert_trailing_zeros_to_spaces(scratchpad);
    if(scratchpad[7]=='.')
    {
      scratchpad[7] = ' ';
    }
    scratchpad[8] = 0;
    fprintf(outputfile, "%s", scratchpad);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "-1      ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    snprintf(scratchpad, 256, "%f", edfhdr->edfparam[signalslist[i]].phys_max);
    convert_trailing_zeros_to_spaces(scratchpad);
    if(scratchpad[7]=='.')
    {
      scratchpad[7] = ' ';
    }
    scratchpad[8] = 0;
    fprintf(outputfile, "%s", scratchpad);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "1       ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].dig_min);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "-32768  ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "-8388608");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].dig_max);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "32767   ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "8388607 ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].prefilter);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<80; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].smp_per_record);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-8i", annot_smp_per_record);
  }
  for(i=0; i<(new_edfsignals * 32); i++)
  {
   fputc(' ', outputfile);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<32; i++)
    {
      fputc(' ', outputfile);
    }
  }
///////////////////////////////////////////////////////////////////

  progress.setRange(0, datarecords);
  progress.setValue(0);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  fseeko(inputfile, (long long)edfhdr->hdrsize + ((long long)(spinBox1->value() - 1) * (long long)edfhdr->recordsize), SEEK_SET);

  for(datrecs_processed=0; datrecs_processed<datarecords; datrecs_processed++)
  {
    if(!(datrecs_processed % progress_steps))
    {
      progress.setValue(datrecs_processed);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto END_4;
      }
    }

    if(fread(readbuf, edfhdr->recordsize, 1, inputfile) != 1)
    {
      progress.reset();
      showpopupmessage("Error", "Read error (2).");
      goto END_4;
    }

    if(edfhdr->edf)
    {
      for(i=0; i<new_edfsignals; i++)
      {
        smplrt = edfhdr->edfparam[signalslist[i]].smp_per_record;

        for(j=0; j<smplrt; j++)
        {
          fputc(readbuf[edfhdr->edfparam[signalslist[i]].buf_offset + (j * 2)], outputfile);
          if(fputc(readbuf[edfhdr->edfparam[signalslist[i]].buf_offset + (j * 2) + 1], outputfile)==EOF)
          {
            progress.reset();
            showpopupmessage("Error", "Write error (4).");
            goto END_4;
          }
        }
      }
    }
    else
    {
      for(i=0; i<new_edfsignals; i++)
      {
        smplrt = edfhdr->edfparam[signalslist[i]].smp_per_record;

        for(j=0; j<smplrt; j++)
        {
          fputc(readbuf[edfhdr->edfparam[signalslist[i]].buf_offset + (j * 3)], outputfile);
          fputc(readbuf[edfhdr->edfparam[signalslist[i]].buf_offset + (j * 3) + 1], outputfile);
          if(fputc(readbuf[edfhdr->edfparam[signalslist[i]].buf_offset + (j * 3) + 2], outputfile)==EOF)
          {
            progress.reset();
            showpopupmessage("Error", "Write error (4).");
            goto END_4;
          }
        }
      }
    }

    if(edfhdr->edfplus || edfhdr->bdfplus)
    {
      switch(timestamp_decimals)
      {
        case 0 : tallen = fprintf(outputfile, "+%i", (int)(taltime / TIME_DIMENSION));
                  break;
        case 1 : tallen = fprintf(outputfile, "+%i.%01i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 1000000LL));
                  break;
        case 2 : tallen = fprintf(outputfile, "+%i.%02i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 100000LL));
                  break;
        case 3 : tallen = fprintf(outputfile, "+%i.%03i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 10000LL));
                  break;
        case 4 : tallen = fprintf(outputfile, "+%i.%04i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 1000LL));
                  break;
        case 5 : tallen = fprintf(outputfile, "+%i.%05i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 100LL));
                  break;
        case 6 : tallen = fprintf(outputfile, "+%i.%06i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 10LL));
                  break;
        case 7 : tallen = fprintf(outputfile, "+%i.%07i", (int)(taltime / TIME_DIMENSION), (int)(taltime % TIME_DIMENSION));
                  break;
      }

      fputc(20, outputfile);
      fputc(20, outputfile);
      fputc(0, outputfile);

      tallen += 3;

      if(annot_cnt < annot_list_sz)
      {
        for(i=0; i<annots_per_datrec; i++)
        {
          if(annot_cnt >= annot_list_sz)  break;

          annot_ptr = edfplus_annotation_get_item(&new_annot_list, annot_cnt++);

          len = snprintf(scratchpad, 256, "%+i.%07i",
          (int)(annot_ptr->onset / TIME_DIMENSION),
          (int)(annot_ptr->onset % TIME_DIMENSION));

          for(j=0; j<7; j++)
          {
            if(scratchpad[len - j - 1] != '0')
            {
              break;
            }
          }

          if(j)
          {
            len -= j;

            if(j == 7)
            {
              len--;
            }
          }

          if(fwrite(scratchpad, len, 1, outputfile) != 1)
          {
            progress.reset();
            showpopupmessage("Error", "Write error (5).");
            goto END_4;
          }

          tallen += len;

          if(annot_ptr->duration[0]!=0)
          {
            fputc(21, outputfile);
            tallen++;

            tallen += fprintf(outputfile, "%s", annot_ptr->duration);
          }

          fputc(20, outputfile);
          tallen++;

          tallen += fprintf(outputfile, "%s", annot_ptr->annotation);

          fputc(20, outputfile);
          fputc(0, outputfile);
          tallen += 2;
        }
      }

      for(k=tallen; k<annot_recordsize; k++)
      {
        fputc(0, outputfile);
      }

      taltime += edfhdr->long_data_record_duration;
    }
  }

  progress.reset();
  showpopupmessage("Ready", "Done.");

END_4:

  fclose(outputfile);
  outputfile = NULL;

END_3:

END_2:

  if(readbuf != NULL)
  {
    free(readbuf);
    readbuf = NULL;
  }

END_1:

  inputfile = NULL;

  inputpath[0] = 0;

  label1->clear();
  label4->clear();
  label5->clear();

  file_num = -1;

  edfhdr = NULL;

  edfplus_annotation_empty_list(&new_annot_list);

  for(i=0; i<MAXSIGNALS; i++)
  {
    if(block_reader[i] != NULL)
    {
      delete block_reader[i];
    }
  }
}


void UI_ExportFilteredSignalsWindow::showpopupmessage(const char *str1, const char *str2)
{
  QMessageBox messagewindow(QMessageBox::NoIcon, str1, str2);

  messagewindow.exec();
}


void UI_ExportFilteredSignalsWindow::populate_tree_view()
{
  int i, j,
      type,
      model,
      order;

  char txtbuf[2048];

  double frequency,
         frequency2,
         ripple;

  QStandardItem *parentItem,
                *signalItem,
                *filterItem;

  t_model->clear();

  parentItem = t_model->invisibleRootItem();

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->filenum != file_num)  continue;

    txtbuf[0] = 0;

    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      strcpy(txtbuf, "alias: ");
      strcat(txtbuf, mainwindow->signalcomp[i]->alias);
      strcat(txtbuf, "   ");
    }

    for(j=0; j<mainwindow->signalcomp[i]->num_of_signals; j++)
    {
      sprintf(txtbuf + strlen(txtbuf), "%+ix %s",
              mainwindow->signalcomp[i]->factor[j],
              mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[j]].label);

      remove_trailing_spaces(txtbuf);

      strcat(txtbuf, "   ");
    }

    signalItem = new QStandardItem(txtbuf);

    parentItem->appendRow(signalItem);

    filterItem = new QStandardItem("Filters");

    signalItem->appendRow(filterItem);

    if(mainwindow->signalcomp[i]->spike_filter)
    {
      sprintf(txtbuf, "Spike: %.8f", mainwindow->signalcomp[i]->spike_filter_velocity);

      remove_trailing_zeros(txtbuf);

      sprintf(txtbuf + strlen(txtbuf), " %s/0.5mSec.  Hold-off: %i mSec.",
              mainwindow->signalcomp[i]->physdimension,
              mainwindow->signalcomp[i]->spike_filter_holdoff);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    for(j=0; j<mainwindow->signalcomp[i]->filter_cnt; j++)
    {
      if(mainwindow->signalcomp[i]->filter[j]->is_LPF == 1)
      {
        sprintf(txtbuf, "LPF: %fHz", mainwindow->signalcomp[i]->filter[j]->cutoff_frequency);
      }

      if(mainwindow->signalcomp[i]->filter[j]->is_LPF == 0)
      {
        sprintf(txtbuf, "HPF: %fHz", mainwindow->signalcomp[i]->filter[j]->cutoff_frequency);
      }

      remove_trailing_zeros(txtbuf);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    for(j=0; j<mainwindow->signalcomp[i]->ravg_filter_cnt; j++)
    {
      if(mainwindow->signalcomp[i]->ravg_filter_type[j] == 0)
      {
        sprintf(txtbuf, "highpass moving average %i smpls", mainwindow->signalcomp[i]->ravg_filter[j]->size);
      }

      if(mainwindow->signalcomp[i]->ravg_filter_type[j] == 1)
      {
        sprintf(txtbuf, "lowpass moving average %i smpls", mainwindow->signalcomp[i]->ravg_filter[j]->size);
      }

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    for(j=0; j<mainwindow->signalcomp[i]->fidfilter_cnt; j++)
    {
      type = mainwindow->signalcomp[i]->fidfilter_type[j];

      model = mainwindow->signalcomp[i]->fidfilter_model[j];

      frequency = mainwindow->signalcomp[i]->fidfilter_freq[j];

      frequency2 = mainwindow->signalcomp[i]->fidfilter_freq2[j];

      order = mainwindow->signalcomp[i]->fidfilter_order[j];

      ripple = mainwindow->signalcomp[i]->fidfilter_ripple[j];

      if(type == 0)
      {
        if(model == 0)
        {
          sprintf(txtbuf, "highpass Butterworth %fHz %ith order", frequency, order);
        }

        if(model == 1)
        {
          sprintf(txtbuf, "highpass Chebyshev %fHz %ith order %fdB ripple", frequency, order, ripple);
        }

        if(model == 2)
        {
          sprintf(txtbuf, "highpass Bessel %fHz %ith order", frequency, order);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          sprintf(txtbuf, "lowpass Butterworth %fHz %ith order", frequency, order);
        }

        if(model == 1)
        {
          sprintf(txtbuf, "lowpass Chebyshev %fHz %ith order %fdB ripple", frequency, order, ripple);
        }

        if(model == 2)
        {
          sprintf(txtbuf, "lowpass Bessel %fHz %ith order", frequency, order);
        }
      }

      if(type == 2)
      {
        sprintf(txtbuf, "notch %fHz Q-factor %i", frequency, order);
      }

      if(type == 3)
      {
        if(model == 0)
        {
          sprintf(txtbuf, "bandpass Butterworth %f-%fHz %ith order", frequency, frequency2, order);
        }

        if(model == 1)
        {
          sprintf(txtbuf, "bandpass Chebyshev %f-%fHz %ith order %fdB ripple", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          sprintf(txtbuf, "bandpass Bessel %f-%fHz %ith order", frequency, frequency2, order);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          sprintf(txtbuf, "bandstop Butterworth %f-%fHz %ith order", frequency, frequency2, order);
        }

        if(model == 1)
        {
          sprintf(txtbuf, "bandstop Chebyshev %f-%fHz %ith order %fdB ripple", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          sprintf(txtbuf, "bandstop Bessel %f-%fHz %ith order", frequency, frequency2, order);
        }
      }

      remove_trailing_zeros(txtbuf);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    if(mainwindow->signalcomp[i]->ecg_filter != NULL)
    {
      sprintf(txtbuf, "ECG heartrate detection");

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    if(mainwindow->signalcomp[i]->plif_ecg_filter != NULL)
    {
      sprintf(txtbuf, "Powerline interference removal: %iHz",
              (mainwindow->signalcomp[i]->plif_ecg_subtract_filter_plf * 10) + 50);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    if(mainwindow->signalcomp[i]->zratio_filter != NULL)
    {
      sprintf(txtbuf, "Z-ratio  cross-over frequency is %.1f Hz", mainwindow->signalcomp[i]->zratio_crossoverfreq);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }
  }

  tree->setModel(t_model);

  tree->expandAll();
}


















