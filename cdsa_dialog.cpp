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

  long long samples_in_file;

  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signal_comp;

  sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_int;
  if(!sf)
  {
    sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_f + 0.5;
  }

  samples_in_file = (long long)signalcomp->edfhdr->datarecords * (long long)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

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
  if(samples_in_file > 300)
  {
    samples_in_file = 300;
  }
  segmentlen_spinbox->setMaximum((int)samples_in_file);
  segmentlen_spinbox->setValue(mainwindow->cdsa_segmentlen);

  blocklen_label = new QLabel(myobjectDialog);
  blocklen_label->setGeometry(20, 65, 150, 25);
  blocklen_label->setText("Block length");

  blocklen_spinbox = new QSpinBox(myobjectDialog);
  blocklen_spinbox->setGeometry(170, 65, 100, 25);
  blocklen_spinbox->setSuffix(" sec");
  blocklen_spinbox->setMinimum(1);
  blocklen_spinbox->setMaximum(10);
  blocklen_spinbox->setValue(mainwindow->cdsa_blocklen);

  overlap_label = new QLabel(myobjectDialog);
  overlap_label->setGeometry(20, 110, 150, 25);
  overlap_label->setText("Overlap");

  overlap_combobox = new QComboBox(myobjectDialog);
  overlap_combobox->setGeometry(170, 110, 100, 25);
  overlap_combobox->addItem(" 0 %");
  overlap_combobox->addItem("50 %");
  overlap_combobox->addItem("67 %");
  overlap_combobox->addItem("75 %");
  overlap_combobox->addItem("80 %");
  overlap_combobox->setCurrentIndex(mainwindow->cdsa_overlap -1);

  windowfunc_label = new QLabel(myobjectDialog);
  windowfunc_label->setGeometry(20, 155, 150, 25);
  windowfunc_label->setText("Window");

  windowfunc_combobox = new QComboBox(myobjectDialog);
  windowfunc_combobox->setGeometry(170, 155, 100, 25);
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
  windowfunc_combobox->setCurrentIndex(mainwindow->cdsa_window_func);

  min_hz_label = new QLabel(myobjectDialog);
  min_hz_label->setGeometry(20, 200, 150, 25);
  min_hz_label->setText("Min. freq.");

  min_hz_spinbox = new QSpinBox(myobjectDialog);
  min_hz_spinbox->setGeometry(170, 200, 100, 25);
  min_hz_spinbox->setSuffix(" Hz");
  min_hz_spinbox->setMinimum(0);
  min_hz_spinbox->setMaximum((sf / 2) - 1);
  min_hz_spinbox->setValue(mainwindow->cdsa_min_hz);

  max_hz_label = new QLabel(myobjectDialog);
  max_hz_label->setGeometry(20, 245, 150, 25);
  max_hz_label->setText("Max. freq.");

  max_hz_spinbox = new QSpinBox(myobjectDialog);
  max_hz_spinbox->setGeometry(170, 245, 100, 25);
  max_hz_spinbox->setSuffix(" Hz");
  max_hz_spinbox->setMinimum(1);
  max_hz_spinbox->setMaximum(sf / 2);
  max_hz_spinbox->setValue(mainwindow->cdsa_max_hz);

  max_pwr_label = new QLabel(myobjectDialog);
  max_pwr_label->setGeometry(20, 290, 150, 25);
  max_pwr_label->setText("Max. level");

  strlcpy(str, " ", 128);
  strlcat(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 128);
  remove_trailing_spaces(str);

  max_pwr_spinbox = new QDoubleSpinBox(myobjectDialog);
  max_pwr_spinbox->setGeometry(170, 290, 150, 25);
  max_pwr_spinbox->setSuffix(str);
  max_pwr_spinbox->setDecimals(3);
  max_pwr_spinbox->setMinimum(0.001);
  max_pwr_spinbox->setMaximum(10000.0);
  max_pwr_spinbox->setValue(mainwindow->cdsa_max_pwr);

  log_label = new QLabel(myobjectDialog);
  log_label->setGeometry(20, 335, 150, 25);
  log_label->setText("Logarithmic");

  log_checkbox = new QCheckBox(myobjectDialog);
  log_checkbox->setGeometry(170, 335, 20, 25);
  log_checkbox->setTristate(false);
  if(mainwindow->cdsa_log)
  {
    log_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    log_checkbox->setCheckState(Qt::Unchecked);
  }

  close_button = new QPushButton(myobjectDialog);
  close_button->setGeometry(20, 435, 100, 25);
  close_button->setText("Close");

  default_button = new QPushButton(myobjectDialog);
  default_button->setGeometry(250, 435, 100, 25);
  default_button->setText("Default");

  start_button = new QPushButton(myobjectDialog);
  start_button->setGeometry(480, 435, 100, 25);
  start_button->setText("Start");

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


UI_cdsa_window::~UI_cdsa_window()
{
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
  overlap_combobox->setCurrentIndex(4);
  windowfunc_combobox->setCurrentIndex(9);
  min_hz_spinbox->setValue(1);
  max_hz_spinbox->setValue(30);
  max_pwr_spinbox->setValue(50.0);
  log_checkbox->setCheckState(Qt::Checked);

  mainwindow->cdsa_segmentlen = 30;
  mainwindow->cdsa_blocklen = 2;
  mainwindow->cdsa_overlap = 5;
  mainwindow->cdsa_window_func = 9;
  mainwindow->cdsa_min_hz = 1;
  mainwindow->cdsa_max_hz = 30;
  mainwindow->cdsa_max_pwr = 50;
  mainwindow->cdsa_log = 1;

  QObject::blockSignals(false);
}


void UI_cdsa_window::start_button_clicked()
{
  int i, j, w, h, h_min, h_max, err,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      blocklen,
      smpl_in_block,
      overlap,
      window_func,
      log_density=0;

  long long samples_in_file;

  int rgb_map[1786][3],
      rgb_idx;

  double v_scale,
         d_tmp,
         *smplbuf=NULL;

  struct fft_wrap_settings_struct *dft;

  QPixmap *pxm=NULL;

  struct cdsa_dock_param_struct dock_param;

  for(i=0; i<256; i++)
  {
    rgb_map[i][0] = i;
    rgb_map[i][1] = 0;
    rgb_map[i][2] = i;
  }

  for(i=256; i<511; i++)
  {
    rgb_map[i][0] = 510 - i;
    rgb_map[i][1] = 0;
    rgb_map[i][2] = 255;
  }

  for(i=511; i<766; i++)
  {
    rgb_map[i][0] = 0;
    rgb_map[i][1] = i - 510;
    rgb_map[i][2] = 255;
  }

  for(i=766; i<1021; i++)
  {
    rgb_map[i][0] = 0;
    rgb_map[i][1] = 255;
    rgb_map[i][2] = 1020 - i;
  }

  for(i=1021; i<1276; i++)
  {
    rgb_map[i][0] = i - 1020;
    rgb_map[i][1] = 255;
    rgb_map[i][2] = 0;
  }

  for(i=1276; i<1531; i++)
  {
    rgb_map[i][0] = 255;
    rgb_map[i][1] = 1530 - i;
    rgb_map[i][2] = 0;
  }

  for(i=1531; i<1786; i++)
  {
    rgb_map[i][0] = 255;
    rgb_map[i][1] = i - 1530;
    rgb_map[i][2] = i - 1530;
  }

  if(log_checkbox->checkState() == Qt::Checked)
  {
    log_density = 1;
  }
  mainwindow->cdsa_log = log_density;

  samples_in_file = (long long)signalcomp->edfhdr->datarecords * (long long)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  segmentlen = segmentlen_spinbox->value();
  mainwindow->cdsa_segmentlen = segmentlen;

  blocklen = blocklen_spinbox->value();
  mainwindow->cdsa_blocklen = blocklen;

  h_min = min_hz_spinbox->value();
  mainwindow->cdsa_min_hz = h_min;

  h_max = max_hz_spinbox->value();
  mainwindow->cdsa_max_hz = h_max;

  if(log_density)
  {
    v_scale = 1785.0 / log10(max_pwr_spinbox->value());
  }
  else
  {
    v_scale = 1785.0 / max_pwr_spinbox->value();
  }
  mainwindow->cdsa_max_pwr = max_pwr_spinbox->value();

  window_func = windowfunc_combobox->currentIndex();
  mainwindow->cdsa_window_func = window_func;

  overlap = overlap_combobox->currentIndex() + 1;
  mainwindow->cdsa_overlap = overlap;

  smpls_in_segment = sf * segmentlen;

  smpl_in_block = sf * blocklen;

  segments_in_recording = samples_in_file / (long long)smpls_in_segment;

  w = segments_in_recording;

  h_max *= blocklen;

  h_min *= blocklen;

  h = h_max - h_min;

  FilteredBlockReadClass fbr;

  smplbuf = fbr.init_signalcomp(signalcomp, smpls_in_segment, 0, 1);
  if(smplbuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (-1)");
    messagewindow.exec();
    return;
  }

  pxm = new QPixmap(w, h);
  pxm->fill(Qt::black);

  QPainter painter(pxm);

  dft = fft_wrap_create(smplbuf, smpls_in_segment, smpl_in_block, window_func, overlap);
  if(dft == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (-2)");
    messagewindow.exec();
    return;
  }

  QProgressDialog progress("Processing...", "Abort", 0, segments_in_recording);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(500);

  for(i=0; i<segments_in_recording; i++)
  {
    progress.setValue(i);

    qApp->processEvents();

    if(progress.wasCanceled() == true)
    {
      painter.end();
      progress.reset();
      free_fft_wrap(dft);
      delete pxm;
      return;
    }

    err = fbr.process_signalcomp(i * smpls_in_segment);
    if(err)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (-3)");
      messagewindow.exec();
      return;
    }

    fft_wrap_run(dft);

    dft->buf_out[0] /= 2.0;  // DC!

    for(j=0; j<h; j++)
    {
      if(log_density)
      {
        d_tmp = sqrt(dft->buf_out[j + h_min] / dft->dft_sz);

        if(d_tmp < 1E-13)
        {
          rgb_idx = log10(1E-13) * v_scale;
        }
        else
        {
          rgb_idx = log10(d_tmp) * v_scale;
        }
      }
      else
      {
        rgb_idx = sqrt(dft->buf_out[j + h_min] / dft->dft_sz) * v_scale;
      }

      if(rgb_idx > 1785)  rgb_idx = 1785;

      if(rgb_idx < 0)  rgb_idx = 0;

      painter.setPen(QColor(rgb_map[rgb_idx][0], rgb_map[rgb_idx][1], rgb_map[rgb_idx][2]));

      painter.drawPoint(i, (h - 1) - j);
    }
  }

  painter.end();

  progress.reset();

  free_fft_wrap(dft);

  dock_param.signalcomp = signalcomp;
  dock_param.sf = sf;
  dock_param.min_hz = mainwindow->cdsa_min_hz;
  dock_param.max_hz = mainwindow->cdsa_max_hz;
  dock_param.max_pwr = mainwindow->cdsa_max_pwr;
  dock_param.log = mainwindow->cdsa_log;
  dock_param.pxm = pxm;
  dock_param.segment_len = segmentlen;
  dock_param.segments_in_recording = segments_in_recording;

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(mainwindow->cdsa_dock[i] == NULL)
    {
      mainwindow->cdsa_dock[i] = new UI_cdsa_dock(mainwindow, dock_param);

      break;
    }
  }

  pxm = NULL;

  myobjectDialog->close();
}





























