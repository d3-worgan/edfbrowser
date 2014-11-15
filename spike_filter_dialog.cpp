/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/



#include "spike_filter_dialog.h"



UI_SpikeFilterDialog::UI_SpikeFilterDialog(QWidget *w_parent)
{
  int i;

  QListWidgetItem *item;


  mainwindow = (UI_Mainwindow *)w_parent;

  spikefilterdialog = new QDialog;

  spikefilterdialog->setMinimumSize(QSize(620, 365));
  spikefilterdialog->setMaximumSize(QSize(620, 365));
  spikefilterdialog->setWindowTitle("Add a spike filter");
  spikefilterdialog->setModal(true);
  spikefilterdialog->setAttribute(Qt::WA_DeleteOnClose, true);

  velocityLabel = new QLabel(spikefilterdialog);
  velocityLabel->setGeometry(20, 45, 60, 20);
  velocityLabel->setText("Velocity");

  velocitySpinBox = new QDoubleSpinBox(spikefilterdialog);
  velocitySpinBox->setGeometry(90, 45, 190, 20);
  velocitySpinBox->setDecimals(6);
  velocitySpinBox->setSuffix(" units/Sec");
  velocitySpinBox->setMinimum(0.0001);
  velocitySpinBox->setMaximum(10E9);
  velocitySpinBox->setValue(2000000.0);

  holdOffLabel = new QLabel(spikefilterdialog);
  holdOffLabel->setGeometry(20, 90, 60, 20);
  holdOffLabel->setText("Hold-off");

  holdOffSpinBox = new QSpinBox(spikefilterdialog);
  holdOffSpinBox->setGeometry(90, 90, 190, 20);
  holdOffSpinBox->setSuffix(" milliSec");
  holdOffSpinBox->setMinimum(10);
  holdOffSpinBox->setMaximum(1000);
  holdOffSpinBox->setValue(100);

  listlabel = new QLabel(spikefilterdialog);
  listlabel->setGeometry(440, 20, 100, 20);
  listlabel->setText("Select signals:");

  list = new QListWidget(spikefilterdialog);
  list->setGeometry(440, 45, 160, 300);
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  CancelButton = new QPushButton(spikefilterdialog);
  CancelButton->setGeometry(300, 320, 100, 25);
  CancelButton->setText("&Cancel");

  ApplyButton = new QPushButton(spikefilterdialog);
  ApplyButton->setGeometry(20, 320, 100, 25);
  ApplyButton->setText("&Apply");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    item = new QListWidgetItem;
    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      item->setText(mainwindow->signalcomp[i]->alias);
    }
    else
    {
      item->setText(mainwindow->signalcomp[i]->signallabel);
    }
    item->setData(Qt::UserRole, QVariant(i));
    list->addItem(item);
  }

  list->setCurrentRow(0);

  QObject::connect(ApplyButton,  SIGNAL(clicked()), this,              SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton, SIGNAL(clicked()), spikefilterdialog, SLOT(close()));

  spikefilterdialog->exec();
}


void UI_SpikeFilterDialog::ApplyButtonClicked()
{
  int i, s, n, holdoff;

  double sf, velocity;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  if(!n)
  {
    spikefilterdialog->close();
    return;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    if(mainwindow->signalcomp[s]->spike_filter_cnt > MAXFILTERS - 1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The maximum amount of spike filters per signal has been reached.\n"
                                                                         "Remove some spike filters first.");
      messagewindow.exec();
      return;
    }
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = list->row(item);

    sf = (double)(mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].smp_per_record) /
         mainwindow->signalcomp[s]->edfhdr->data_record_duration;

    if(sf < 1000.0)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Spike filters can only be used when the samplerate is at least 1000Hz.");
      messagewindow.exec();
      spikefilterdialog->close();
      return;
    }

    velocity = velocitySpinBox->value() /
               mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].bitvalue;

    holdoff = holdOffSpinBox->value();

    mainwindow->signalcomp[s]->spike_filter[mainwindow->signalcomp[s]->spike_filter_cnt] = create_spike_filter(sf, velocity, holdoff);
    if(mainwindow->signalcomp[s]->spike_filter[mainwindow->signalcomp[s]->spike_filter_cnt] == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a spike filter.");
      messagewindow.exec();
      spikefilterdialog->close();
      return;
    }

    mainwindow->signalcomp[s]->spike_filter_velocity[mainwindow->signalcomp[s]->spike_filter_cnt] = velocitySpinBox->value();

    mainwindow->signalcomp[s]->spike_filter_holdoff[mainwindow->signalcomp[s]->spike_filter_cnt] = holdOffSpinBox->value();

    mainwindow->signalcomp[s]->spike_filter_cnt++;
  }

  spikefilterdialog->close();

  mainwindow->setup_viewbuf();
}










