/*******************************************************************************
* fred Copyright (c) 2011-2013 by Gillen Daniel <gillen.dan@pinguin.lu>        *
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

#include <QList>
#include <QStringList>
#include <QVariant>
#include <QDateTime>

#include <inttypes.h>

/*******************************************************************************
 * Public
 ******************************************************************************/

RegistryNodeTreeModel::RegistryNodeTreeModel(RegistryHive *p_hive,
                                             QObject *p_parent)
  : QAbstractItemModel(p_parent)
{
  // Create root node. It's values will be used as header values.
  this->p_root_node=new RegistryNode(QList<QVariant>()<<tr("Node")
                                                      <<tr("Last mod. time"));

  // Load data
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

  switch(role) {
    case Qt::DisplayRole: {
      switch(index.column()) {
        case RegistryNodeTreeModel::ColumnContent_NodeName: {
          return p_node->Data(index.column());
          break;
        }
        case RegistryNodeTreeModel::ColumnContent_NodeModTime: {
          QDateTime date_time;
          bool ok=false;
          date_time.setTimeSpec(Qt::UTC);
          date_time.setTime_t(RegistryHive::FiletimeToUnixtime(
                                p_node->Data(index.column()).toLongLong(&ok)));
          if(ok) return date_time.toString("yyyy/MM/dd hh:mm:ss");
          else return tr("Unknown");
          break;
        }
        default: {
          return QVariant();
        }
      }
      break;
    }
    default: {
      return QVariant();
    }
  }

  // Control will never reach this, but in order to stop g++ complaining...
  return QVariant();
}

Qt::ItemFlags RegistryNodeTreeModel::flags(const QModelIndex &index) const {
  if(!index.isValid()) return 0;
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant RegistryNodeTreeModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
  // Only horizontal header is supported
  if(orientation!=Qt::Horizontal) return QVariant();

  switch(role) {
    case Qt::TextAlignmentRole:
      // Handle text alignment
      return Qt::AlignCenter;
      break;
    case Qt::DisplayRole:
      // Header text
      return this->p_root_node->Data(section);
      break;
    default:
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

  RegistryNode *p_child_node=p_parent_node->Child(row);
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
  RegistryNode *p_parent_node=p_child_node->Parent();

  if(p_parent_node==this->p_root_node) {
    return QModelIndex();
  } else {
    return this->createIndex(p_parent_node->Row(),0,p_parent_node);
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

  return p_parent_node->ChildCount();
}

int RegistryNodeTreeModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2;
}

QList<QModelIndex> RegistryNodeTreeModel::GetIndexListOf(QString path) const {
  RegistryNode *p_parent_node=this->p_root_node;
  QList<QModelIndex> ret;
  QStringList nodes=path.split("\\",QString::SkipEmptyParts);
  bool found=false;

  // Create a list of all index's that form the supplied path
  ret.clear();
  for(int i=0;i<nodes.count();i++) {
    found=false;
    for(uint64_t ii=0;ii<p_parent_node->ChildCount();ii++) {
      if(p_parent_node->Child(ii)->Data(0)==nodes.at(i)) {
        ret.append(this->createIndex(ii,0,p_parent_node->Child(ii)));
        p_parent_node=p_parent_node->Child(ii);
        found=true;
        break;
      }
    }
    // Break when last child node was found
    if(found && i==nodes.count()-1) break;
    // Return an empty list when a child node wasn't found
    else if(!found) return QList<QModelIndex>();
  }

  return ret;
}

QString RegistryNodeTreeModel::GetNodePath(QModelIndex child_index) const
{
  QString path;

  // Get current node name
  path=this->data(child_index,Qt::DisplayRole).toString().prepend("\\");
  // Build node path by prepending all parent nodes names
  while(this->parent(child_index)!=QModelIndex()) {
    child_index=this->parent(child_index);
    path.prepend(this->data(child_index,
                            Qt::DisplayRole).toString().prepend("\\"));
  }

  return path;
}

QModelIndex RegistryNodeTreeModel::AddNode(const QModelIndex &parent_index,
                                           int new_node_id,
                                           QString new_node_name)
{
  RegistryNode *p_parent_node;
  int64_t key_mod_time;
  RegistryNode *p_node;

  // Get pointer to parent node
  p_parent_node=static_cast<RegistryNode*>(parent_index.internalPointer());

  // Tell users of this view that we are going to insert a row
  emit(RegistryNodeTreeModel::beginInsertRows(parent_index,
                                              p_parent_node->ChildCount(),
                                              p_parent_node->ChildCount()));

  // Create and add new node in internal node list
  key_mod_time=p_hive->GetNodeModTime(new_node_id);
  p_node=new RegistryNode(QList<QVariant>()<<new_node_name<<
                            QVariant((qlonglong)key_mod_time),
                          p_parent_node);
  p_parent_node->AppendChild(p_node);

  // Tell users of this view we have finished inserting a row
  emit(RegistryNodeTreeModel::endInsertRows());

  // Return index to new node
  return this->createIndex(p_parent_node->ChildCount()-1,0,p_node);
}

/*******************************************************************************
 * Private
 ******************************************************************************/

void RegistryNodeTreeModel::SetupModelData(RegistryHive *p_hive,
                                           RegistryNode *p_parent,
                                           int hive_node)
{
  QMap<QString,int> hive_children;
  RegistryNode *p_node;
  int64_t key_mod_time;

  // Get all sub nodes of current hive node
  if(hive_node) hive_children=p_hive->GetNodes(hive_node);
  else hive_children=p_hive->GetNodes("\\");
  if(hive_children.isEmpty()) return;

  // Recursivly iterate over all sub nodes
  QMapIterator<QString, int> i(hive_children);
  while(i.hasNext()) {
    i.next();
    key_mod_time=p_hive->GetNodeModTime(i.value());
    // TODO: Maybe we have to call GetErrorMsg in case an error occured
    p_node=new RegistryNode(QList<QVariant>()<<i.key()<<
                              QVariant((qlonglong)key_mod_time),p_parent);
    p_parent->AppendChild(p_node);
    this->SetupModelData(p_hive,p_node,i.value());
  }
}
