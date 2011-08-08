/*******************************************************************************
* fred Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>             *
*                                                                              *
* Forensic Registry EDitor (fred) is a M$ registry hive editor for Linux with  *
* special feautures useful during forensic analysis.                           *
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

#include "registrynodetreemodel.h"

#include <stdlib.h>

RegistryNodeTreeModel::RegistryNodeTreeModel(hive_h *hhive,
                                             hive_node_h hive_root_node,
                                             QObject *p_parent)
  : QAbstractItemModel(p_parent)
{
  Q_UNUSED(p_parent);
  this->p_root_node=new RegistryNode("ROOT");
  this->SetupModelData(hhive,
                       hive_root_node,
                       this->p_root_node);
}

RegistryNodeTreeModel::~RegistryNodeTreeModel() {
  delete this->p_root_node;
}

QVariant RegistryNodeTreeModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) return QVariant();
  if(role!=Qt::DisplayRole) return QVariant();

  RegistryNode *p_node=static_cast<RegistryNode*>(index.internalPointer());

  return p_node->data();
}

Qt::ItemFlags RegistryNodeTreeModel::flags(const QModelIndex &index) const {
  if(!index.isValid()) return 0;
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant RegistryNodeTreeModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
  Q_UNUSED(section);
  if(orientation==Qt::Horizontal && role==Qt::DisplayRole) {
    return QVariant("Registry key folders");
  } else {
    return QVariant();
  }
}

QModelIndex RegistryNodeTreeModel::index(int row,
                                         int column,
                                         const QModelIndex &parent) const
{
  if(!this->hasIndex(row,column,parent)) return QModelIndex();

  RegistryNode *p_parent_node;
  if(!parent.isValid()) {
    p_parent_node=this->p_root_node;
  } else {
    p_parent_node=static_cast<RegistryNode*>(parent.internalPointer());
  }

  RegistryNode *p_child_node=p_parent_node->child(row);
  if(p_child_node) {
    return this->createIndex(row,column,p_child_node);
  } else {
    return QModelIndex();
  }
}

QModelIndex RegistryNodeTreeModel::parent(const QModelIndex &index) const {
  if(!index.isValid()) return QModelIndex();

  RegistryNode *p_child_node=
    static_cast<RegistryNode*>(index.internalPointer());
  RegistryNode *p_parent_node=p_child_node->parent();

  if(p_parent_node==this->p_root_node) {
    return QModelIndex();
  } else {
    return this->createIndex(p_parent_node->row(),0,p_parent_node);
  }
}

int RegistryNodeTreeModel::rowCount(const QModelIndex &parent) const {
  if(parent.column()>0) return 0;

  RegistryNode *p_parent_node;
  if(!parent.isValid()) {
    p_parent_node=this->p_root_node;
  } else {
    p_parent_node=static_cast<RegistryNode*>(parent.internalPointer());
  }

  return p_parent_node->childCount();
}

int RegistryNodeTreeModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 1;
}

void RegistryNodeTreeModel::SetupModelData(hive_h *hhive,
                                           hive_node_h hive_node,
                                           RegistryNode *p_parent)
{
  int i=0;
  RegistryNode *p_node;

  // Get all sub nodes of current hive node
  hive_node_h *p_hive_children=hivex_node_children(hhive,hive_node);
  if(p_hive_children==NULL) return;

  // Recursivly iterate over all sub nodes
  while(p_hive_children[i]) {
    p_node=new RegistryNode(QString(hivex_node_name(hhive,p_hive_children[i])),
                            p_parent);
    p_parent->AppendChild(p_node);
    this->SetupModelData(hhive,p_hive_children[i],p_node);
    i++;
  }
  free(p_hive_children);
}
