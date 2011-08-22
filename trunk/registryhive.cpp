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

#include "registryhive.h"

#include <QStringList>
#include <QDateTime>

#include <stdlib.h>

/*
 * RegistryHive
 */
RegistryHive::RegistryHive(QObject *p_parent) : QObject(p_parent) {
  this->erro_msg="";
  this->is_error=false;
  this->hive_file="";
  this->p_hive=NULL;
  this->is_hive_open=false;
}

/*
 * ~RegistryHive
 */
RegistryHive::~RegistryHive() {
  if(this->is_hive_open) this->Close();
}

/*
 * Error
 */
bool RegistryHive::Error() {
  return this->is_error;
}

/*
 * GetErrorMsg
 */
QString RegistryHive::GetErrorMsg() {
  QString msg=this->erro_msg;
  this->erro_msg="";
  this->is_error=false;
  return msg;
}

/*
 * Open
 */
bool RegistryHive::Open(QString file, bool read_only) {
  if(this->is_hive_open) return false;

  // Open hive file
  this->p_hive=hivex_open(file.toAscii().constData(),
                          read_only ? 0 : HIVEX_OPEN_WRITE);
  if(this->p_hive==NULL) return false;

  // Set local vars
  this->hive_file=file;
  this->is_hive_open=true;

  return true;
}

/*
 * Close
 */
bool RegistryHive::Close(bool commit_changes) {
  if(this->is_hive_open) {
    if(commit_changes) {
      // Commit changes before closing hive.
      // TODO: Maybe it would be more secure to commit changes to a new file and
      // then move it over the original one.
      hivex_commit(this->p_hive,NULL,0);
    }

    // As hivex_close will _ALWAYS_ free the handle, we don't need the following
    // values anymore
    this->hive_file="";
    this->is_hive_open=false;

    // Close hive
    if(hivex_close(this->p_hive)!=0) return false;
  }
  return true;
}

/*
 * GetNodes
 */
QMap<QString,int> RegistryHive::GetNodes(QString path) {
  hive_node_h parent_node;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&parent_node)) return QMap<QString,int>();

  // Get and return nodes
  return this->GetNodesHelper(parent_node);
}

/*
 * GetNodes
 */
QMap<QString,int> RegistryHive::GetNodes(int parent_node) {
  if(parent_node==0) {
    this->SetError(tr("Invalid parent node handle specified!"));
    return QMap<QString,int>();
  }

  // Get and return nodes
  return this->GetNodesHelper(parent_node);
}

/*
 * GetKeys
 */
QMap<QString,int> RegistryHive::GetKeys(QString path) {
  hive_node_h parent_node;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&parent_node)) return QMap<QString,int>();

  // Get and return keys
  return this->GetKeysHelper(parent_node);
}

/*
 * GetKeys
 */
QMap<QString,int> RegistryHive::GetKeys(int parent_node) {
  if(parent_node==0) {
    this->SetError(tr("Invalid parent node handle specified!"));
    return QMap<QString,int>();
  }

  // Get and return keys
  return this->GetKeysHelper(parent_node);
}

/*
 * GetKeyValue
 */
QByteArray RegistryHive::GetKeyValue(QString path,
                                     QString key,
                                     int *p_value_type,
                                     size_t *p_value_len)
{
  hive_node_h parent_node;
  hive_value_h hive_key;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&parent_node)) return QByteArray();

  // Get key handle
  hive_key=hivex_node_get_value(this->p_hive,
                                parent_node,key.toAscii().constData());
  if(hive_key==0) {
    this->SetError(tr("Unable to get key handle!"));
    *p_value_len=-1;
    return QByteArray();
  }

  // Get and return key value
  return this->GetKeyValueHelper(hive_key,p_value_type,p_value_len);
}

/*
 * GetKeyValue
 */
QByteArray RegistryHive::GetKeyValue(int hive_key,
                                     int *p_value_type,
                                     size_t *p_value_len)
{
  if(hive_key==0) {
    this->SetError(tr("Invalid key handle specified!"));
    *p_value_type=-1;
    return QByteArray();
  }

  // Get and return key value
  return this->GetKeyValueHelper(hive_key,p_value_type,p_value_len);
}

/*
 * KeyValueToString
 */
QString RegistryHive::KeyValueToString(QByteArray value, int value_type) {
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
#if __WORDSIZE == 64
      ret=QString().sprintf("0x%016lX",*(uint64_t*)value.constData());
#else
      ret=QString().sprintf("0x%016llX",*(uint64_t*)value.constData());
#endif
      break;
    default:
      ToHexStr();
  }

  #undef ToHexStr

  return ret;
}

/*
 * KeyValueToString
 */
QString RegistryHive::KeyValueToString(QByteArray key_value,
                                       QString format,
                                       int offset,
                                       int length)
{
  int remaining_data_len;
  const char *p_data;
  QString ret="";

  // Calculate how many bytes are remainig after specified offset
  remaining_data_len=key_value.size()-offset;
  if(!remaining_data_len>0) {
    // Nothing to show
    return QString();
  }

  // Get pointer to data at specified offset
  p_data=key_value.constData();
  p_data+=offset;

  // ConvertFull name
  if(format=="int8" && remaining_data_len>=1) {
    ret=QString().sprintf("%d",*(int8_t*)p_data);
  } else if(format=="uint8" && remaining_data_len>=1) {
    ret=QString().sprintf("%u",*(uint8_t*)p_data);
  } else if(format=="int16" && remaining_data_len>=2) {
    ret=QString().sprintf("%d",*(int16_t*)p_data);
  } else if(format=="uint16" && remaining_data_len>=2) {
    ret=QString().sprintf("%u",*(uint16_t*)p_data);
  } else if(format=="int32" && remaining_data_len>=4) {
    ret=QString().sprintf("%d",*(int32_t*)p_data);
  } else if(format=="uint32" && remaining_data_len>=4) {
    ret=QString().sprintf("%u",*(uint32_t*)p_data);
  } else if(format=="unixtime" && remaining_data_len>=4) {
    if(*(uint32_t*)p_data==0) {
      ret="n/a";
    } else {
      QDateTime date_time;
      date_time.setTime_t(*(uint32_t*)p_data);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(format=="int64" && remaining_data_len>=8) {
#if __WORDSIZE == 64
    ret=QString().sprintf("%ld",*(int64_t*)p_data);
#else
    ret=QString().sprintf("%lld",*(int64_t*)p_data);
#endif
  } else if(format=="uint64" && remaining_data_len>=8) {
#if __WORDSIZE == 64
    ret=QString().sprintf("%lu",*(uint64_t*)p_data);
#else
    ret=QString().sprintf("%llu",*(uint64_t*)p_data);
#endif
  } else if(format=="filetime" && remaining_data_len>=8) {
    if(*(uint64_t*)p_data==0) {
      ret="n/a";
    } else {
      // TODO: Warn if >32bit
      QDateTime date_time;
      date_time.setTime_t((*(uint64_t*)p_data-116444736000000000)/10000000);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(format=="ascii") {
    // TODO: This fails bad if the string is not null terminated!! It might be
    // wise checking for a null char here
    ret=QString().fromAscii((char*)p_data,length);
  } else if(format=="utf16" && remaining_data_len>=2) {
    ret=QString().fromUtf16((ushort*)p_data,length);
  } else {
    // Unknown variant type or another error
    return QString();
  }

  return ret;
}

/*
 * KeyTypeToString
 */
QString RegistryHive::KeyTypeToString(int value_type) {
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

/*
 * SetError
 */
void RegistryHive::SetError(QString msg) {
  this->erro_msg=msg;
  this->is_error=true;
}

/*
 * GetNodeHandle
 */
bool RegistryHive::GetNodeHandle(QString &path, hive_node_h *p_node) {
  QStringList nodes;
  int i=0;

  // Get root node handle
  *p_node=hivex_root(this->p_hive);
  if(*p_node==0) {
    this->SetError(tr("Unable to get root node!"));
    return false;
  }

  if(path!="\\") {
    // If we aren't listing the root node, we have to get a handle to the
    // last node in the path. Split path into nodes
    nodes=path.split('\\',QString::SkipEmptyParts);

    // Iterate to the correct parent node
    for(i=0;i<nodes.count();i++) {
      *p_node=hivex_node_get_child(this->p_hive,
                                   *p_node,
                                   nodes.value(i).toAscii().constData());
      if(*p_node==0) {
        this->SetError(tr("Unable to find node '%1'!").arg(nodes.value(i)));
        return false;
      }
    }
  }

  return true;
}

/*
 * GetNodesHelper
 */
QMap<QString,int> RegistryHive::GetNodesHelper(hive_node_h parent_node) {
  QMap<QString,int> keys;
  char *p_name;
  int i=0;

  // Get child nodes
  hive_node_h *child_nodes=hivex_node_children(this->p_hive,parent_node);
  if(child_nodes==NULL) {
    this->SetError(
      tr("Unable to enumerate child nodes!"));
    return QMap<QString,int>();
  }

  // Build result
  keys.clear();
  i=0;
  while(child_nodes[i]) {
    p_name=hivex_node_name(this->p_hive,child_nodes[i]);
    if(p_name==NULL) {
      this->SetError(tr("Unable to get node name!"));
      free(child_nodes);
      return QMap<QString,int>();
    }
    keys.insert(QString(p_name),(int)child_nodes[i]);
    free(p_name);
    i++;
  }
  free(child_nodes);

  return keys;
}

/*
 * GetKeysHelper
 */
QMap<QString,int> RegistryHive::GetKeysHelper(hive_node_h parent_node) {
  QMap<QString,int> keys;
  char *p_name;
  int i=0;

  // Get child keys
  hive_value_h *p_keys=hivex_node_values(this->p_hive,parent_node);
  if(p_keys==NULL) {
    this->SetError(
      tr("Unable to enumerate child keys!"));
    return QMap<QString,int>();
  }

  // Build result list
  keys.clear();
  i=0;
  while(p_keys[i]) {
    p_name=hivex_value_key(this->p_hive,p_keys[i]);
    if(p_name==NULL) {
      this->SetError(tr("Unable to get key name!"));
      return QMap<QString,int>();
    }
    keys.insert(QString(p_name),p_keys[i]);
    free(p_name);
    i++;
  }
  free(p_keys);

  return keys;
}

QByteArray RegistryHive::GetKeyValueHelper(hive_value_h hive_key,
                                           int *p_value_type,
                                           size_t *p_value_len)
{
  QByteArray key_value;
  char *p_key_value;

  p_key_value=hivex_value_value(this->p_hive,
                                hive_key,
                                (hive_type*)p_value_type,
                                p_value_len);
  if(p_key_value==NULL) {
    this->SetError(tr("Unable to get key value!"));
    *p_value_type=-1;
    return QByteArray();
  }

  // Feed QByteArray and free p_key_value
  key_value=QByteArray(p_key_value,*p_value_len);
  free(p_key_value);

  return key_value;
}
