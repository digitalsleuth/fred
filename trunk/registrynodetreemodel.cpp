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
#include "registrynodetreemodel.h"

#include <QStringList>

#include <stdlib.h>

RegistryNodeTreeModel::RegistryNodeTreeModel(RegistryHive *p_hive,
                                             QObject *p_parent)
  : QAbstractItemModel(p_parent)
{
  // Create root node
  this->p_root_node=new RegistryNode("ROOT");
  // Build node list
  this->SetupModelData(p_hive,this->p_root_node);
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

QList<QModelIndex> RegistryNodeTreeModel::GetIndexListOf(QString &path) const {
  RegistryNode *p_parent_node=this->p_root_node;
  QList<QModelIndex> ret;
  QStringList nodes=path.split("\\",QString::SkipEmptyParts);
  int i,ii;
  bool found=false;

  // Create a list of all index's that form the supplied path
  ret.clear();
  for(i=0;i<nodes.count();i++) {
    found=false;
    for(ii=0;ii<p_parent_node->childCount();ii++) {
      if(p_parent_node->child(ii)->data()==nodes.at(i)) {
        ret.append(this->createIndex(ii,0,p_parent_node->child(ii)));
        p_parent_node=p_parent_node->child(ii);
        found=true;
        break;
      }
    }
    if(!found) return QList<QModelIndex>();
  }

  return ret;
}

void RegistryNodeTreeModel::SetupModelData(RegistryHive *p_hive,
                                           RegistryNode *p_parent,
                                           int hive_node)
{
  QMap<QString,int> hive_children;
  RegistryNode *p_node;

  // Get all sub nodes of current hive node
  if(hive_node) hive_children=p_hive->GetNodes(hive_node);
  else hive_children=p_hive->GetNodes("\\");
  if(hive_children.isEmpty()) return;

  // Recursivly iterate over all sub nodes
  QMapIterator<QString, int> i(hive_children);
  while(i.hasNext()) {
    i.next();
    p_node=new RegistryNode(i.key(),p_parent);
    p_parent->AppendChild(p_node);
    this->SetupModelData(p_hive,p_node,i.value());
  }
}
