/*******************************************************************************
* fred Copyright (c) 2011-2012 by Gillen Daniel <gillen.dan@pinguin.lu>        *
*                                                                              *
* Forensic Registry EDitor (fred) is a cross-platform M$ registry hive editor  *
* with special feautures useful during forensic analysis.                      *
*                                                                              *
* This program is free software: you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation, either version 3 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program. If not, see <http://www.gnu.org/licenses/>.                    *
*******************************************************************************/

#include "registrykeytable.h"

#include <QHeaderView>
#include <QApplication>
#include <QClipboard>

RegistryKeyTable::RegistryKeyTable(QWidget *p_parent) : QTableView(p_parent) {
  // Configure widget
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setAutoScroll(false);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->verticalHeader()->setHidden(true);
  this->setTextElideMode(Qt::ElideNone);

  // Create context menu
  this->p_menu_copy=new QMenu(tr("Copy"),this);
  this->p_action_copy_key_name=new QAction(tr("Key name"),
                                           this->p_menu_copy);
  this->p_menu_copy->addAction(this->p_action_copy_key_name);
  this->connect(this->p_action_copy_key_name,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyKeyName()));
  this->p_action_copy_key_value=new QAction(tr("Key value"),
                                            this->p_menu_copy);
  this->p_menu_copy->addAction(this->p_action_copy_key_value);
  this->connect(this->p_action_copy_key_value,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyKeyValue()));
}

RegistryKeyTable::~RegistryKeyTable() {
  // Delete context menu
  delete this->p_action_copy_key_name;
  delete this->p_action_copy_key_value;
  delete this->p_menu_copy;
}

void RegistryKeyTable::setModel(QAbstractItemModel *p_model) {
  QTableView::setModel(p_model);
  // Resize table rows / columns to fit data
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
  this->horizontalHeader()->stretchLastSection();
  if(p_model!=NULL && p_model->rowCount()>0) {
    // Select first table item
    this->selectRow(0);
  }
}

/*
void RegistryKeyTable::selectRow(QString key_name) {
  int i;

  this->clearSelection();
  for(i=0;i<this->model()->rowCount();i++) {
    if(this->model())
  }
}
*/

int RegistryKeyTable::sizeHintForColumn(int column) const {
  int size_hint=-1;
  int i=0;
  int item_width=0;
  QFontMetrics fm(this->fontMetrics());
  QModelIndex idx;

  if(this->model()==NULL) return -1;

  // Find string that needs the most amount of space
  idx=this->model()->index(i,column);
  while(idx.isValid()) {
    item_width=fm.width(this->model()->data(idx).toString())+10;
    if(item_width>size_hint) size_hint=item_width;
    idx=this->model()->index(++i,column);
  }

  return size_hint;
}

void RegistryKeyTable::contextMenuEvent(QContextMenuEvent *p_event) {
  // Only show context menu when a row is selected
  if(this->selectedIndexes().count()!=3) return;
  // Only show context menu when user clicked on selected row
  if(!(this->indexAt(p_event->pos())==this->selectedIndexes().at(0) ||
       this->indexAt(p_event->pos())==this->selectedIndexes().at(1) ||
       this->indexAt(p_event->pos())==this->selectedIndexes().at(2)))
  {
    return;
  }

  // Emit a click signal
  emit(this->clicked(this->indexAt(p_event->pos())));

  // Create context menu and add actions
  QMenu context_menu(this);
  context_menu.addMenu(this->p_menu_copy);
  context_menu.exec(p_event->globalPos());
}

void RegistryKeyTable::currentChanged(const QModelIndex &current,
                                      const QModelIndex &previous)
{
  // Call parent class's currentChanged first
  QTableView::currentChanged(current,previous);

  // Now emit our signal
  QModelIndex current_item=QModelIndex(current);
  emit(RegistryKeyTable::CurrentItemChanged(current_item));
}

void RegistryKeyTable::SlotCopyKeyName() {
  QApplication::clipboard()->
    setText(this->selectedIndexes().at(0).data().toString(),
            QClipboard::Clipboard);
}

void RegistryKeyTable::SlotCopyKeyValue() {
  QApplication::clipboard()->
    setText(this->selectedIndexes().at(2).data().toString(),
            QClipboard::Clipboard);
}
