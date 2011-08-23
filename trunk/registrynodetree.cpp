/*******************************************************************************
* fred Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>             *
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
