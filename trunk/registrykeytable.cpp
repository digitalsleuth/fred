#include "registrykeytable.h"

#include <QHeaderView>

RegistryKeyTable::RegistryKeyTable(QWidget *p_parent) : QTableView(p_parent) {
  // Configure widget
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void RegistryKeyTable::setModel(QAbstractItemModel *p_model) {
  QTableView::setModel(p_model);
  // Resize table rows / columns to fit data
  this->resizeColumnsToContents();
  this->horizontalHeader()->stretchLastSection();
}

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
