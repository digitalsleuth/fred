#include "registrynodetree.h"

#include <QHeaderView>

RegistryNodeTree::RegistryNodeTree(QWidget *p_parent) : QTreeView(p_parent) {
  // Configure widget
  this->setTextElideMode(Qt::ElideNone);
}

void RegistryNodeTree::setModel(QAbstractItemModel *p_model) {
  QTreeView::setModel(p_model);
  this->header()->setResizeMode(0,QHeaderView::ResizeToContents);
  this->header()->setStretchLastSection(false);
}

//int RegistryNodeTree::sizeHintForColumn(int column) const {}
