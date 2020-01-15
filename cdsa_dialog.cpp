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
  char str[128]={""};

  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signal_comp;

  sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_int;
  if(!sf)
  {
    sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_f + 0.5;
  }

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Color Density Spectral Array");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  segmentlen_label = new QLabel(myobjectDialog);
  segmentlen_label->setGeometry(20, 20, 150, 25);
  segmentlen_label->setText("Segment length");

  segmentlen_spinbox = new QSpinBox(myobjectDialog);
  segmentlen_spinbox->setGeometry(170, 20, 100, 25);
  segmentlen_spinbox->setSuffix(" sec");
  segmentlen_spinbox->setMinimum(5);
  segmentlen_spinbox->setMaximum(300);

  blocklen_label = new QLabel(myobjectDialog);
  blocklen_label->setGeometry(20, 65, 150, 25);
  blocklen_label->setText("Block length");

  blocklen_spinbox = new QSpinBox(myobjectDialog);
  blocklen_spinbox->setGeometry(170, 65, 100, 25);
  blocklen_spinbox->setSuffix(" sec");
  blocklen_spinbox->setMinimum(1);
  blocklen_spinbox->setMaximum(10);

  pix_per_hz_label = new QLabel(myobjectDialog);
  pix_per_hz_label->setGeometry(20, 110, 150, 25);
  pix_per_hz_label->setText("Pixels per bin");

  pix_per_hz_spinbox = new QSpinBox(myobjectDialog);
  pix_per_hz_spinbox->setGeometry(170, 110, 100, 25);
  pix_per_hz_spinbox->setSuffix(" px");
  pix_per_hz_spinbox->setMinimum(1);
  pix_per_hz_spinbox->setMaximum(10);

  overlap_label = new QLabel(myobjectDialog);
  overlap_label->setGeometry(20, 155, 150, 25);
  overlap_label->setText("Overlap");

  overlap_combobox = new QComboBox(myobjectDialog);
  overlap_combobox->setGeometry(170, 155, 100, 25);
  overlap_combobox->addItem(" 0 %");
  overlap_combobox->addItem("50 %");
  overlap_combobox->addItem("67 %");
  overlap_combobox->addItem("75 %");
  overlap_combobox->addItem("80 %");

  windowfunc_label = new QLabel(myobjectDialog);
  windowfunc_label->setGeometry(20, 200, 150, 25);
  windowfunc_label->setText("Window");

  windowfunc_combobox = new QComboBox(myobjectDialog);
  windowfunc_combobox->setGeometry(170, 200, 100, 25);
  windowfunc_combobox->addItem("Rectangular");
  windowfunc_combobox->addItem("Hamming");
  windowfunc_combobox->addItem("4-term Blackman-Harris");
  windowfunc_combobox->addItem("7-term Blackman-Harris");
  windowfunc_combobox->addItem("Nuttall3b");
  windowfunc_combobox->addItem("Nuttall4c");
  windowfunc_combobox->addItem("Hann");
  windowfunc_combobox->addItem("HFT223D");
  windowfunc_combobox->addItem("HFT95");
  windowfunc_combobox->addItem("Kaiser2");
  windowfunc_combobox->addItem("Kaiser3");
  windowfunc_combobox->addItem("Kaiser4");
  windowfunc_combobox->addItem("Kaiser5");

  min_hz_label = new QLabel(myobjectDialog);
  min_hz_label->setGeometry(20, 245, 150, 25);
  min_hz_label->setText("Min. freq.");

  min_hz_spinbox = new QSpinBox(myobjectDialog);
  min_hz_spinbox->setGeometry(170, 245, 100, 25);
  min_hz_spinbox->setSuffix(" Hz");
  min_hz_spinbox->setMinimum(0);
  min_hz_spinbox->setMaximum((sf / 2) - 1);

  max_hz_label = new QLabel(myobjectDialog);
  max_hz_label->setGeometry(20, 290, 150, 25);
  max_hz_label->setText("Max. freq.");

  max_hz_spinbox = new QSpinBox(myobjectDialog);
  max_hz_spinbox->setGeometry(170, 290, 100, 25);
  max_hz_spinbox->setSuffix(" Hz");
  max_hz_spinbox->setMinimum(1);
  max_hz_spinbox->setMaximum(sf / 2);

  max_pwr_label = new QLabel(myobjectDialog);
  max_pwr_label->setGeometry(20, 335, 150, 25);
  max_pwr_label->setText("Max. power");

  strlcpy(str, " ", 128);
  strlcat(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 128);
  remove_trailing_spaces(str);
  strlcat(str, "^2", 128);

  max_pwr_spinbox = new QDoubleSpinBox(myobjectDialog);
  max_pwr_spinbox->setGeometry(170, 335, 150, 25);
  max_pwr_spinbox->setSuffix(str);
  max_pwr_spinbox->setDecimals(3);
  max_pwr_spinbox->setMinimum(0.001);
  max_pwr_spinbox->setMaximum(10000.0);

  close_button = new QPushButton(myobjectDialog);
  close_button->setGeometry(20, 435, 100, 25);
  close_button->setText("Close");

  default_button = new QPushButton(myobjectDialog);
  default_button->setGeometry(250, 435, 100, 25);
  default_button->setText("Default");

  start_button = new QPushButton(myobjectDialog);
  start_button->setGeometry(480, 435, 100, 25);
  start_button->setText("Start");

  default_button_clicked();

  QObject::connect(close_button, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  if(sf >= 60)
  {
    QObject::connect(default_button,     SIGNAL(clicked()),         this, SLOT(default_button_clicked()));
    QObject::connect(start_button,       SIGNAL(clicked()),         this, SLOT(start_button_clicked()));
    QObject::connect(segmentlen_spinbox, SIGNAL(valueChanged(int)), this, SLOT(segmentlen_spinbox_changed(int)));
    QObject::connect(blocklen_spinbox,   SIGNAL(valueChanged(int)), this, SLOT(blocklen_spinbox_changed(int)));
    QObject::connect(min_hz_spinbox,     SIGNAL(valueChanged(int)), this, SLOT(min_hz_spinbox_changed(int)));
    QObject::connect(max_hz_spinbox,     SIGNAL(valueChanged(int)), this, SLOT(max_hz_spinbox_changed(int)));
  }

  myobjectDialog->exec();
}


void UI_cdsa_window::segmentlen_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(blocklen_spinbox->value() > (value / 3))
  {
    blocklen_spinbox->setValue(value / 3);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::blocklen_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(segmentlen_spinbox->value() < (value * 3))
  {
    segmentlen_spinbox->setValue(value * 3);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::min_hz_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(max_hz_spinbox->value() <= value)
  {
    max_hz_spinbox->setValue(value + 1);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::max_hz_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(min_hz_spinbox->value() >= value)
  {
    min_hz_spinbox->setValue(value - 1);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::default_button_clicked()
{
  QObject::blockSignals(true);

  segmentlen_spinbox->setValue(30);
  blocklen_spinbox->setValue(2);
  pix_per_hz_spinbox->setValue(2);
  overlap_combobox->setCurrentIndex(4);
  windowfunc_combobox->setCurrentIndex(9);
  min_hz_spinbox->setValue(1);
  max_hz_spinbox->setValue(30);
  max_pwr_spinbox->setValue(50.0);

  QObject::blockSignals(false);
}


void UI_cdsa_window::start_button_clicked()
{
  int i, err,
      datrecs_in_segment,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      blocklen,
      smpl_in_block,
      overlap,
      window_func;

  char str[1024]={""};

  struct fft_wrap_settings_struct *dft;

  segmentlen = segmentlen_spinbox->value();

  blocklen = blocklen_spinbox->value();

  window_func = windowfunc_combobox->currentIndex();

  overlap = overlap_combobox->currentIndex() + 1;

  datrecs_in_segment = (segmentlen * TIME_DIMENSION) / signalcomp->edfhdr->long_data_record_duration;

  segments_in_recording = signalcomp->edfhdr->datarecords / datrecs_in_segment;

  FilteredBlockReadClass fbr;

  smplbuf = fbr.init_signalcomp(signalcomp, datrecs_in_segment, 0);
  if(smplbuf == NULL)
  {
    snprintf(str, 1024, "Internal error in file %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    return;
  }

  smpls_in_segment = fbr.samples_in_buf();

  smpl_in_block = sf * blocklen;

  dft = fft_wrap_create(smplbuf, smpls_in_segment, smpl_in_block, window_func, overlap);
  if(dft == NULL)
  {
    snprintf(str, 1024, "Internal error in file %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    return;
  }

  for(i=0; i<segments_in_recording; i++)
  {
    err = fbr.process_signalcomp(datrecs_in_segment);
    if(err)
    {
      snprintf(str, 1024, "Internal error %i in file %s line %i", err, __FILE__, __LINE__);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      return;
    }

    fft_wrap_run(dft);
  }





  free_fft_wrap(dft);
}

























