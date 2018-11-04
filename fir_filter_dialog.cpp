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



#include "fir_filter_dialog.h"



UI_FIRFilterDialog::UI_FIRFilterDialog(QWidget *w_parent)
{
  int i, n, s;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  mainwindow = (UI_Mainwindow *)w_parent;

  firfilterdialog = new QDialog;

  firfilterdialog->setMinimumSize(620, 415);
  firfilterdialog->setMaximumSize(620, 415);
  firfilterdialog->setWindowTitle("Add a FIR filter");
  firfilterdialog->setModal(true);
  firfilterdialog->setAttribute(Qt::WA_DeleteOnClose, true);

  varsLabel = new QLabel(firfilterdialog);
  varsLabel->setGeometry(20, 20, 100, 25);
  varsLabel->setText("Taps");

  listlabel = new QLabel(firfilterdialog);
  listlabel->setGeometry(440, 20, 100, 25);
  listlabel->setText("Select signals:");

  textEdit = new QTextEdit(firfilterdialog);
  textEdit->setGeometry(20, 45, 250, 300);
  textEdit->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  textEdit->setReadOnly(false);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);

  list = new QListWidget(firfilterdialog);
  list->setGeometry(440, 45, 160, 300);
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  CancelButton = new QPushButton(firfilterdialog);
  CancelButton->setGeometry(20, 370, 100, 25);
  CancelButton->setText("&Close");

  ApplyButton = new QPushButton(firfilterdialog);
  ApplyButton->setGeometry(300, 370, 100, 25);
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

  n = list->count();

  for(i=0; i<n; i++)
  {
    item = list->item(i);
    s = item->data(Qt::UserRole).toInt();

    item->setSelected(false);
  }

  QObject::connect(ApplyButton,  SIGNAL(clicked()),     this,            SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton, SIGNAL(clicked()),     firfilterdialog, SLOT(close()));
  QObject::connect(textEdit,     SIGNAL(textChanged()), this,            SLOT(check_text()));

  firfilterdialog->exec();
}


void UI_FIRFilterDialog::check_text()
{
  strncpy(textbuf, textEdit->toPlainText().toLatin1().data(), 100000);

//  strtok_r_e(
}


void UI_FIRFilterDialog::ApplyButtonClicked()
{
  int i, s, n;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    if(mainwindow->signalcomp[s]->fir_filter)
    {
      free_fir_filter(mainwindow->signalcomp[s]->fir_filter);

      mainwindow->signalcomp[s]->fir_filter = NULL;
    }

//    mainwindow->signalcomp[s]->fir_filter = create_fir_filter();
    if(mainwindow->signalcomp[s]->fir_filter == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a FIR filter.");
      messagewindow.exec();
    }
  }

  mainwindow->setup_viewbuf();
}










