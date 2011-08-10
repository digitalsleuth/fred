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

#include "registrykeytablemodel.h"

#include <stdlib.h>

RegistryKeyTableModel::RegistryKeyTableModel(hive_h *hhive,
                                             hive_node_h hive_node,
                                             QObject *p_parent)
  : QAbstractTableModel(p_parent)
{
  // Create the "root" key. It's values will be used for as header values.
  this->p_keys=new RegistryKey(QList<QVariant>()<<
                                 tr("Key")<<tr("Type")<<tr("Value"));
  // Build key list
  this->SetupModelData(hhive,hive_node);
}

RegistryKeyTableModel::~RegistryKeyTableModel() {
  delete this->p_keys;
}

QVariant RegistryKeyTableModel::data(const QModelIndex &index, int role) const {
  bool ok;

  if(!index.isValid()) return QVariant();

  RegistryKey *p_key=static_cast<RegistryKey*>(index.internalPointer());

  switch(role) {
    case Qt::DisplayRole: {
      switch(index.column()) {
        case RegistryKeyTableModel::ColumnContent_KeyName: {
          return p_key->Data(index.column());
          break;
        }
        case RegistryKeyTableModel::ColumnContent_KeyType: {
          int value_type=p_key->Data(index.column()).toInt(&ok);
          if(!ok) return QVariant();
          return this->TypeToString(value_type);
          break;
        }
        case RegistryKeyTableModel::ColumnContent_KeyValue: {
          // Get index to value type
          QModelIndex type_index=this->index(index.row(),
                                             RegistryKeyTableModel::
                                               ColumnContent_KeyType);
          // Get value type
          int value_type=this->data(type_index,
                                    RegistryKeyTableModel::
                                      AdditionalRoles_GetRawData).toInt(&ok);
          if(!ok) return QVariant();
          // Return value converted to human readeable string
          QByteArray value_array=p_key->Data(index.column()).toByteArray();
          return this->ValueToString(value_array,value_type);
          break;
        }
        default:
          return QVariant();
      }
      break;
    }
    case RegistryKeyTableModel::AdditionalRoles_GetRawData: {
      return p_key->Data(index.column());
      break;
    }
    default:
      return QVariant();
  }

  /*
  if(role!=Qt::DisplayRole) return QVariant();

  RegistryKey *p_key=static_cast<RegistryKey*>(index.internalPointer());

  return p_key->Data(index.column());
  */
}

Qt::ItemFlags RegistryKeyTableModel::flags(const QModelIndex &index) const {
  if(!index.isValid()) return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant RegistryKeyTableModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
  // Only horizontal header is supported
  if(orientation!=Qt::Horizontal) return QVariant();

  switch(role) {
    case Qt::TextAlignmentRole:
      // Handle text alignment
      if(section==2) return Qt::AlignLeft;
      else return Qt::AlignCenter;
      break;
    case Qt::DisplayRole:
      // Header text
      return this->p_keys->Data(section);
      break;
    default:
      return QVariant();
  }
}

QModelIndex RegistryKeyTableModel::index(int row,
                                         int column,
                                         const QModelIndex &parent) const
{
  if(!this->hasIndex(row,column,parent)) return QModelIndex();

  RegistryKey *p_key=this->p_keys->Key(row);

  return this->createIndex(row,column,p_key);
}

int RegistryKeyTableModel::rowCount(const QModelIndex &parent) const {
  // According to Qt doc, when parent in TableModel is valid, we should return 0
  if(parent.isValid()) return 0;
  // Get and return row count from the keys list
  return this->p_keys->RowCount();
}

int RegistryKeyTableModel::columnCount(const QModelIndex &parent) const {
  // According to Qt doc, when parent in TableModel is valid, we should return 0
  if(parent.isValid()) return 0;
  // There are always 3 columns
  return 3;
}

void RegistryKeyTableModel::SetupModelData(hive_h *hhive,
                                           hive_node_h hive_node)
{
  int i=0;
  RegistryKey *p_key;
  char *p_key_name;
  char *key_value;
  int key_value_type;
  size_t key_value_len;

  // Get all (key,value) pairs for current node
  hive_value_h *node_keys=hivex_node_values(hhive,hive_node);
  if(node_keys==NULL) return;

  // Add all (key,value) pairs to the key list
  while(node_keys[i]) {
    // Get key name
    p_key_name=hivex_value_key(hhive,node_keys[i]);
    if(p_key_name==NULL) continue;
    // Get key value and type
    key_value=hivex_value_value(hhive,
                                node_keys[i],
                                (hive_type*)&key_value_type,
                                &key_value_len);
    if(key_value==NULL) continue;

    // Add infos to key list
    p_key=new RegistryKey(QList<QVariant>()<<
                            QString(strlen(p_key_name) ?
                                      p_key_name : "(default)")<<
                            QVariant(key_value_type)<<
                            QByteArray(key_value,key_value_len));
    this->p_keys->Append(p_key);
    i++;
  }
  free(node_keys);
}

QString RegistryKeyTableModel::ValueToString(QByteArray &value,
                                             int value_type) const
{
  QString ret="";
  int i=0;

  #define ToHexStr() {                                                        \
    for(i=0;i<value.size();i++) {                                             \
      ret.append(QString().sprintf("%02X ",(uint8_t)(value.constData()[i]))); \
    }                                                                         \
    ret.chop(1);                                                              \
  }

  switch(value_type) {
    case hive_t_REG_NONE:
      // Just a key without a value, but to be certain...
      ToHexStr();
      break;
    case hive_t_REG_SZ:
      // A Windows string (encoding is unknown, but often UTF16-LE)
      // TODO: What happens if encoding is not UTF16-LE ??? Thx Billy!!!
      ret=value.size() ? QString().fromUtf16((ushort*)(value.constData())) : "";
      break;
    case hive_t_REG_EXPAND_SZ:
      // A Windows string that contains %env% (environment variable expansion)
      ret=value.size() ? QString().fromUtf16((ushort*)(value.constData())) : "";
      break;
    case hive_t_REG_BINARY:
      // A blob of binary
      ToHexStr();
      break;
    case hive_t_REG_DWORD:
      // DWORD (32 bit integer), little endian
      ret=QString().sprintf("0x%08X",*(uint32_t*)value.constData());
      //ret=QString().sprintf("0x%08X",value.toUInt());
      break;
    case hive_t_REG_DWORD_BIG_ENDIAN:
      // DWORD (32 bit integer), big endian
      ret=QString().sprintf("0x%08X",*(uint32_t*)value.constData());
      //ret=QString().sprintf("0x%08X",value.toUInt());
      break;
    case hive_t_REG_LINK:
      // Symbolic link to another part of the registry tree
      ToHexStr();
      break;
    case hive_t_REG_MULTI_SZ:
      // Multiple Windows strings.
      // See http://blogs.msdn.com/oldnewthing/archive/2009/10/08/9904646.aspx
      ToHexStr();
      break;
    case hive_t_REG_RESOURCE_LIST:
      // Resource list
      ToHexStr();
      break;
    case hive_t_REG_FULL_RESOURCE_DESCRIPTOR:
      // Resource descriptor
      ToHexStr();
      break;
    case hive_t_REG_RESOURCE_REQUIREMENTS_LIST:
      // Resouce requirements list
      ToHexStr();
      break;
    case hive_t_REG_QWORD:
      // QWORD (64 bit integer). Usually little endian.
      ret=QString().sprintf("0x%016X",*(uint64_t*)value.constData());
      //ret=QString().sprintf("0x%08X",value.toULongLong());
      break;
    default:
      ToHexStr();
  }

  #undef ToHexStr

  return ret;
}

QString RegistryKeyTableModel::TypeToString(int value_type) const {
  QString ret="";

  switch(value_type) {
    case hive_t_REG_NONE:
      ret="REG_NONE";
      break;
    case hive_t_REG_SZ:
      ret="REG_SZ";
      break;
    case hive_t_REG_EXPAND_SZ:
      ret="REG_EXPAND_SZ";
      break;
    case hive_t_REG_BINARY:
      ret="REG_BINARY";
      break;
    case hive_t_REG_DWORD:
      ret="REG_DWORD";
      break;
    case hive_t_REG_DWORD_BIG_ENDIAN:
      ret="REG_DWORD_BIG_ENDIAN";
      break;
    case hive_t_REG_LINK:
      ret="REG_LINK";
      break;
    case hive_t_REG_MULTI_SZ:
      ret="REG_MULTI_SZ";
      break;
    case hive_t_REG_RESOURCE_LIST:
      ret="REG_RESOURCE_LIST";
      break;
    case hive_t_REG_FULL_RESOURCE_DESCRIPTOR:
      ret="REG_FULL_RESOURCE_DESC";
      break;
    case hive_t_REG_RESOURCE_REQUIREMENTS_LIST:
      ret="REG_RESOURCE_REQ_LIST";
      break;
    case hive_t_REG_QWORD:
      ret="REG_QWORD";
      break;
    default:
      ret=QString().sprintf("0x%08X",(uint32_t)value_type);
  }

  return ret;
}
