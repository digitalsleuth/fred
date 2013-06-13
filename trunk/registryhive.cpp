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

#include "registryhive.h"

#include <QStringList>
#include <QDateTime>

#include <QDebug>

#include <stdlib.h>
#include <stdio.h>

// TODO: __WORDSIZE is not defined under mingw and I currently have no idea how
// to identify a 64bit windows
#ifndef __WORDSIZE
  #define __WORDSIZE 32
#endif

#if __WORDSIZE == 64
  #define EPOCH_DIFF 0x19DB1DED53E8000
#else
  #define EPOCH_DIFF 0x19DB1DED53E8000LL
#endif

/*******************************************************************************
 * Public
 ******************************************************************************/

/*
 * RegistryHive
 */
RegistryHive::RegistryHive(QObject *p_parent) : QObject(p_parent) {
  this->erro_msg="";
  this->is_error=false;
  this->hive_file="";
  this->p_hive=NULL;
  this->is_hive_open=false;
  this->is_hive_writable=false;
  this->has_changes_to_commit=false;
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
  this->is_hive_writable=!read_only;

  return true;
}

/*
 * Reopen
 */
bool RegistryHive::Reopen(bool read_only) {
  if(!this->is_hive_open) return false;

  // Close hive first
  if(hivex_close(this->p_hive)!=0) {
    // According to the docs, even if hivex_close fails, it frees all handles.
    // So we consider this fatal and final!
    this->hive_file="";
    this->is_hive_open=false;
    this->is_hive_writable=false;
    this->has_changes_to_commit=false;
    return false;
  }

  // Reopen same hive
  this->p_hive=hivex_open(this->hive_file.toAscii().constData(),
                          read_only ? 0 : HIVEX_OPEN_WRITE);
  if(this->p_hive==NULL) {
    this->hive_file="";
    this->is_hive_open=false;
    this->is_hive_writable=false;
    this->has_changes_to_commit=false;
    return false;
  }

  // Update local vars
  this->is_hive_writable=!read_only;
  this->has_changes_to_commit=false;

  return true;
}

/*
 * CommitChanges
 */
bool RegistryHive::CommitChanges() {
  if(!this->is_hive_open || !this->is_hive_writable) return false;
  if(!this->has_changes_to_commit) return true;

  // TODO: Maybe it would be more secure to commit changes to a new file and
  // then move it over the original one.
  if(hivex_commit(this->p_hive,NULL,0)!=0) {
    return false;
  }
  return true;
}

/*
 * Close
 */
bool RegistryHive::Close() {
  if(this->is_hive_open) {
    // As hivex_close will _ALWAYS_ free the handle, we don't need the following
    // values anymore
    this->hive_file="";
    this->is_hive_open=false;
    this->is_hive_writable=false;
    this->has_changes_to_commit=false;

    // Close hive
    if(hivex_close(this->p_hive)!=0) return false;
  }
  return true;
}

/*
 * Filename
 */
QString RegistryHive::Filename() {
  if(this->is_hive_open) return this->hive_file;
  return QString();
}

/*
 * HiveType
 */
RegistryHive::teHiveType RegistryHive::HiveType() {
  // Check for SYSTEM hive
  if(this->PathExists("\\Select") && this->PathExists("\\MountedDevices"))
    return RegistryHive::eHiveType_SYSTEM;
  // Check for SOFTWARE hive
  if(this->PathExists("\\Microsoft\\Windows\\CurrentVersion") &&
     this->PathExists("\\Microsoft\\Windows NT\\CurrentVersion"))
    return RegistryHive::eHiveType_SOFTWARE;
  // Check for SAM
  if(this->PathExists("SAM\\Domains\\Account\\Users"))
    return RegistryHive::eHiveType_SAM;
  // Check for SECURITY
  if(this->PathExists("\\Policy\\Accounts") &&
     this->PathExists("\\Policy\\PolAdtEv"))
    return RegistryHive::eHiveType_SECURITY;
  // Check for NTUSER.DAT
  if(this->PathExists("\\Software\\Microsoft\\Windows\\CurrentVersion"))
    return RegistryHive::eHiveType_NTUSER;
  // Unknown hive
  return RegistryHive::eHiveType_UNKNOWN;
}

/*
 * HiveTypeToString
 */
QString RegistryHive::HiveTypeToString(teHiveType hive_type) {
  switch(hive_type) {
    case RegistryHive::eHiveType_SYSTEM:
      return "SYSTEM";
      break;
    case RegistryHive::eHiveType_SOFTWARE:
      return "SOFTWARE";
      break;
    case RegistryHive::eHiveType_SAM:
      return "SAM";
      break;
    case RegistryHive::eHiveType_SECURITY:
      return "SECURITY";
      break;
    case RegistryHive::eHiveType_NTUSER:
      return "NTUSER";
      break;
    default:
      return "UNKNOWN";
  }
}

/*
 * HasChangesToCommit
 */
bool RegistryHive::HasChangesToCommit() {
  return this->has_changes_to_commit;
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
 * GetKeyName
 */
bool RegistryHive::GetKeyName(int hive_key, QString &key_name) {
  char *buf;

  if(!this->is_hive_open) {
    this->SetError(tr("Need to operate on an open hive!"));
    return false;
  }

  buf=hivex_value_key(this->p_hive,(hive_value_h)hive_key);
  if(buf==NULL) {
    this->SetError(tr("Unable to get key name for key '%1'").arg(hive_key));
    return false;
  }

  key_name=QString(buf);
  free(buf);

  return true;
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
 * GetKeyModTime
 */
int64_t RegistryHive::GetNodeModTime(QString path) {
  hive_node_h node;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&node)) {
    this->SetError(tr("Unable to get node handle!"));
    return 0;
  }

  // Get and return node's last modification timestamp
  return this->GetNodeModTime(node);
}

/*
 * GetKeyModTime
 */
int64_t RegistryHive::GetNodeModTime(int node) {
  if(node==0) {
    this->SetError(tr("Invalid node handle specified!"));
    return 0;
  }

  // Get and return key's last modification timestamp
  return hivex_node_timestamp(this->p_hive,node);
}

/*
 * KeyValueToString
 */
QString RegistryHive::KeyValueToString(QByteArray value, int value_type) {
  QString ret="";
  int i=0;

  #define ToHexStr() {                                                      \
    for(i=0;i<value.size();i++) {                                           \
    ret.append(QString().sprintf("%02X ",(uint8_t)(value.constData()[i]))); \
    }                                                                       \
    ret.chop(1);                                                            \
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
      break;
    case hive_t_REG_DWORD_BIG_ENDIAN:
      // DWORD (32 bit integer), big endian
      ret=QString().sprintf("0x%08X",*(uint32_t*)value.constData());
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
      ret=
        QString("0x%1").arg((quint64)(*(uint64_t*)value.constData()),16,16,QChar('0'));
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
                                       int length,
                                       bool little_endian)
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

  // TODO: This will fail on platforms with different endianness!
#define bswap_16(value) ((((value) & 0xff) << 8) | ((value) >> 8))
#define bswap_32(value)                                       \
  (((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
  (uint32_t)bswap_16((uint16_t)((value) >> 16)))
#define bswap_64(value)                                           \
  (((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) << 32) | \
  (uint64_t)bswap_32((uint32_t)((value) >> 32)))                  \

  // Convert full name
  if(format=="int8" && remaining_data_len>=1) {
    ret=QString().sprintf("%d",*(int8_t*)p_data);
  } else if(format=="uint8" && remaining_data_len>=1) {
    ret=QString().sprintf("%u",*(uint8_t*)p_data);
  } else if(format=="int16" && remaining_data_len>=2) {
    int16_t val=*(int16_t*)p_data;
    if(little_endian) ret=QString().sprintf("%d",val);
    else ret=QString().sprintf("%d",bswap_16(val));
  } else if(format=="uint16" && remaining_data_len>=2) {
    uint16_t val=*(uint16_t*)p_data;
    if(little_endian) ret=QString().sprintf("%u",val);
    else ret=QString().sprintf("%u",bswap_16(val));
  } else if(format=="int32" && remaining_data_len>=4) {
    int32_t val=*(int32_t*)p_data;
    if(little_endian) ret=QString().sprintf("%d",val);
    else ret=QString().sprintf("%d",bswap_32(val));
  } else if(format=="uint32" && remaining_data_len>=4) {
    uint32_t val=*(uint32_t*)p_data;
    if(little_endian) ret=QString().sprintf("%u",val);
    else ret=QString().sprintf("%u",bswap_32(val));
  } else if(format=="unixtime" && remaining_data_len>=4) {
    uint32_t val=*(uint32_t*)p_data;
    if(!little_endian) val=bswap_32(val);
    if(val==0) {
      ret="n/a";
    } else {
      QDateTime date_time;
      date_time.setTimeSpec(Qt::UTC);
      date_time.setTime_t(val);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(format=="int64" && remaining_data_len>=8) {
    int64_t val=*(int64_t*)p_data;
    if(little_endian) ret=QString("%1").arg(val);
    else ret=QString("%1").arg((int64_t)bswap_64(val));
  } else if(format=="uint64" && remaining_data_len>=8) {
    uint64_t val=*(uint64_t*)p_data;
    if(little_endian) ret=QString("%1").arg(val);
    else ret=QString("%1").arg(bswap_64(val));
/*
  // TODO: Check how one could implement this
  } else if(format=="unixtime64" && remaining_data_len>=8) {
    if(*(uint64_t*)p_data==0) {
      ret="n/a";
    } else {
      uint64_t secs=*(uint64_t*)p_data;
      QDateTime date_time;
      date_time.setTimeSpec(Qt::UTC);
      // Set 32bit part of date/time
      date_time.setTime_t(secs&0xFFFFFFFF);
      // Now add high 32bit part of date/time
      date_time.addSecs(secs>>32);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
*/
  } else if(format=="filetime" && remaining_data_len>=8) {
    uint64_t val=*(uint64_t*)p_data;
    if(!little_endian) val=bswap_64(val);
    if(val==0) {
      ret="n/a";
    } else {
      // TODO: Warn if >32bit
      QDateTime date_time;
      date_time.setTimeSpec(Qt::UTC);
      date_time.setTime_t(RegistryHive::FiletimeToUnixtime(val));
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
 * KeyValueToStringList
 */
QStringList RegistryHive::KeyValueToStringList(QByteArray value,
                                               int value_type)
{
  QStringList result;
  const char str_sep[2]={0x00,0x00};
  int last_pos=0,cur_pos=0;

  // Only supported on REG_MULTI_SZ values!!
  if(value_type!=hive_t_REG_MULTI_SZ) return QStringList();

  while(last_pos<value.count() &&
        (cur_pos=value.indexOf(QByteArray().fromRawData(str_sep,2),last_pos))!=-1)
  {
    if(cur_pos!=last_pos) {
      // TODO: What happens if encoding is not UTF16-LE ??? Thx Billy!!!
      result.append(QString().fromUtf16((ushort*)value
                                          .mid(last_pos,cur_pos-last_pos)
                                          .append(QByteArray().fromRawData(str_sep,2))
                                          .constData()));
    }
    last_pos=cur_pos+3;
  }

  return result;
}

/*
 * KeyValueToStringList
 */
QStringList RegistryHive::KeyValueToStringList(QByteArray value,
                                               QString value_type)
{
  return RegistryHive::KeyValueToStringList(value,
                                            RegistryHive::StringToKeyValueType(
                                              value_type));
}

/*
 * GetKeyValueTypes
 */
QStringList RegistryHive::GetKeyValueTypes() {
  return QStringList()<<"REG_NONE"
                        <<"REG_SZ"
                        <<"REG_EXPAND_SZ"
                        <<"REG_BINARY"
                        <<"REG_DWORD"
                        <<"REG_DWORD_BIG_ENDIAN"
                        <<"REG_LINK"
                        <<"REG_MULTI_SZ"
                        <<"REG_RESOURCE_LIST"
                        <<"REG_FULL_RESOURCE_DESC"
                        <<"REG_RESOURCE_REQ_LIST"
                        <<"REG_QWORD";
}

/*
 * KeyTypeToString
 */
QString RegistryHive::KeyValueTypeToString(int value_type) {
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
 * StringToKeyValueType
 */
int RegistryHive::StringToKeyValueType(QString value_type) {
  if(value_type=="REG_NONE") return hive_t_REG_NONE;
  if(value_type=="REG_SZ") return hive_t_REG_SZ;
  if(value_type=="REG_EXPAND_SZ") return hive_t_REG_EXPAND_SZ;
  if(value_type=="REG_BINARY") return hive_t_REG_BINARY;
  if(value_type=="REG_DWORD") return hive_t_REG_DWORD;
  if(value_type=="REG_DWORD_BIG_ENDIAN") return hive_t_REG_DWORD_BIG_ENDIAN;
  if(value_type=="REG_LINK") return hive_t_REG_LINK;
  if(value_type=="REG_MULTI_SZ") return hive_t_REG_MULTI_SZ;
  if(value_type=="REG_RESOURCE_LIST") return hive_t_REG_RESOURCE_LIST;
  if(value_type=="REG_FULL_RESOURCE_DESC")
    return hive_t_REG_FULL_RESOURCE_DESCRIPTOR;
  if(value_type=="REG_RESOURCE_REQ_LIST")
    return hive_t_REG_RESOURCE_REQUIREMENTS_LIST;
  if(value_type=="REG_QWORD") return hive_t_REG_QWORD;

  // I think this might be a good default :-)
  return hive_t_REG_BINARY;
}

/*
 * FiletimeToUnixtime
 */
uint64_t RegistryHive::FiletimeToUnixtime(int64_t filetime) {
  return (unsigned)((filetime-EPOCH_DIFF)/10000000);
}

/*
 * AddNode
 */
int RegistryHive::AddNode(QString parent_node_path, QString node_name) {
  if(!this->is_hive_writable) return 0;

  // Make sure name does not contain a backslash char
  if(node_name.contains('\\')) {
    this->SetError(tr("Unable to add node with name '%1'. "
                        "Names can not include a backslash character.")
                     .arg(node_name));
    return 0;
  }

  // Get node handle to the parent where the new node should be created
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    this->SetError(tr("Unable to get node handle for '%1'!")
                     .arg(parent_node_path));
    return 0;
  }

  // Make sure there is no other node with same name
  QMap<QString,int> child_nodes=this->GetNodes(parent_node);
  if(child_nodes.contains(node_name.toAscii())) {
    this->SetError(tr("The node '%1\\%2' already exists!")
                     .arg(parent_node_path,node_name));
    return 0;
  }

  // Add new node
  hive_node_h new_node=hivex_node_add_child(this->p_hive,
                                            parent_node,
                                            node_name.toAscii().constData());
  if(new_node==0) {
    this->SetError(tr("Unable to create new node '%1\\%2'!")
                     .arg(parent_node_path,node_name));
    return 0;
  }

  this->has_changes_to_commit=true;
  return new_node;
}

/*
 * DeleteNode
 */
bool RegistryHive::DeleteNode(QString node_path) {
  if(!this->is_hive_writable) return false;

  // Get node handle to the node that should be deleted
  hive_node_h node;
  if(!this->GetNodeHandle(node_path,&node)) {
    this->SetError(tr("Unable to get node handle for '%1'!")
                     .arg(node_path));
    return false;
  }

  // Delete node
  if(hivex_node_delete_child(this->p_hive,node)==-1) {
    this->SetError(tr("Unable to delete node '%1'!")
                     .arg(node_path));
    return false;
  }

  this->has_changes_to_commit=true;
  return true;
}

/*
 * AddKey
 */
int RegistryHive::AddKey(QString parent_node_path,
                         QString key_name,
                         QString key_value_type,
                         QByteArray key_value)
{
  if(!this->is_hive_open || !this->is_hive_writable) {
    // TODO: Set error
    return false;
  }

  return this->SetKey(parent_node_path,
                      key_name,
                      key_value_type,
                      key_value,
                      true);
}

/*
 * UpdateKey
 */
int RegistryHive::UpdateKey(QString parent_node_path,
                            QString key_name,
                            QString key_value_type,
                            QByteArray key_value)
{
  if(!this->is_hive_open || !this->is_hive_writable) {
    // TODO: Set error
    return false;
  }

  return this->SetKey(parent_node_path,
                      key_name,
                      key_value_type,
                      key_value,
                      false);
}

/*
 * DeleteKey
 */
bool RegistryHive::DeleteKey(QString parent_node_path, QString key_name) {
  if(!this->is_hive_open || !this->is_hive_writable) {
    // TODO: Set error
    return false;
  }

  // libhivex offers no possibility to delete a single key :-(
  // As a work around, this function temporarly stores all keys of the specified
  // node, then deletes them all an re-creates all but the one that should be
  // deleted.

  // Get handle to parent node
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    // TODO: Set error
    return false;
  }

  // Get all child keys
  hive_value_h *p_keys=hivex_node_values(this->p_hive,parent_node);
  if(p_keys==NULL) {
    this->SetError(tr("Unable to enumerate child keys for parent '%1'!")
                     .arg(parent_node_path));
    return false;
  }

  // Get all child key values except the one that should be deleted
  int i=0;
  char *p_name;
  int node_keys_count=0;
  hive_set_value *node_keys=NULL;

#define FREE_NODE_KEYS() {             \
  for(int x=0;x<node_keys_count;x++) { \
    free(node_keys[x].key);            \
    free(node_keys[x].value);          \
  }                                    \
  free(node_keys);                     \
}

  while(p_keys[i]) {
    p_name=hivex_value_key(this->p_hive,p_keys[i]);
    if(p_name==NULL) {
      this->SetError(tr("Unable to get key name for a child of '%1'!")
                       .arg(parent_node_path));
      return false;
    }
    if(QString(p_name)!=key_name) {
      // Current key is not the one that should be deleted, save it
      // Alloc mem for new hive_set_value struct in node_keys array
      node_keys=(hive_set_value*)realloc(node_keys,
                                         sizeof(hive_set_value)*
                                           (node_keys_count+1));
      if(node_keys==NULL) {
        this->SetError(tr("Unable to alloc enough memory for all child keys!"));
        return false;
      }
      // Save key name in hive_set_value struct
      node_keys[node_keys_count].key=p_name;
      // Get key value, key value type and key value len and save to
      // hive_set_value struct
      node_keys[node_keys_count].value=
        hivex_value_value(this->p_hive,
                          p_keys[i],
                          &(node_keys[node_keys_count].t),
                          &(node_keys[node_keys_count].len));
      if(node_keys[node_keys_count].value==NULL) {
        this->SetError(tr("Unable to get value for key '%1'!").arg(p_name));
        free(p_name);
        // Free all temporary stored keys
        FREE_NODE_KEYS();
        return false;
      }
      node_keys_count++;
    } else {
      // Current key is to be deleted, ignore it
      free(p_name);
    }
    i++;
  }

  // Save all stored keys to hive, which will discard the one that should be
  // deleted
  if(hivex_node_set_values(this->p_hive,
                           parent_node,
                           node_keys_count,
                           node_keys,
                           0)!=0)
  {
    this->SetError(tr("Unable to re-save all child keys! Please discard any "
                      "changes you made and start over. No doing so might end "
                      "in data loss!"));
    // Free all temporary stored keys
    FREE_NODE_KEYS();
    return false;
  }

  // Free all temporary stored keys and return
  FREE_NODE_KEYS();

#undef FREE_NODE_KEYS

  this->has_changes_to_commit=true;
  return true;
}

/*******************************************************************************
 * Private
 ******************************************************************************/

/*
 * HivexError2String
 */
QString RegistryHive::HivexError2String(int error) {
  switch(error) {
    case ENOTSUP:
      return QString("Corrupt or unsupported Registry file format.");
      break;
    case HIVEX_NO_KEY:
      return QString("Missing root key.");
      break;
    case EINVAL:
      return QString("Passed an invalid argument to the function.");
      break;
    case EFAULT:
      return QString("Followed a Registry pointer which goes outside the "
                       "registry or outside a registry block.");
      break;
    case ELOOP:
      return QString("Registry contains cycles.");
      break;
    case ERANGE:
      return QString("Field in the registry out of range.");
      break;
    case EEXIST:
      return QString("Registry key already exists.");
      break;
    case EROFS:
      return QString("Tried to write to a registry which is not opened for "
                       "writing.");
      break;
    default:
      return QString("Unknown error.");
  }
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
 * GetKeyHandle
 */
bool RegistryHive::GetKeyHandle(QString &parent_node_path,
                                QString &key_name,
                                hive_value_h *p_key)
{
  // Get handle to parent node
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    // TODO: Set error
    return false;
  }

  // Get handle to key
  *p_key=hivex_node_get_value(this->p_hive,
                              parent_node,
                              key_name.toAscii().constData());
  if(*p_key==0) {
    // TODO: Set error
    return false;
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

/*
 * GetKeyValueHelper
 */
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

/*
 * PathExists
 */
bool RegistryHive::PathExists(QString path) {
  bool ret;
  hive_node_h node;

  ret=this->GetNodeHandle(path,&node);
  if(!ret || this->Error()) {
    // Clear error and return false
    this->GetErrorMsg();
    return false;
  }

  return true;
}

/*
 * SetKey
 */
int RegistryHive::SetKey(QString &parent_node_path,
                         QString &key_name,
                         QString &key_value_type,
                         QByteArray &key_value,
                         bool create_key)
{
  // Get node handle to the node that holds the key to create/update
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    // TODO: Set error
    return 0;
  }

  // Make sure key exists if we should update it
  if(!create_key) {
    hive_value_h temp_key=hivex_node_get_value(this->p_hive,
                                               parent_node,
                                               key_name.toAscii().constData());
    if(temp_key==0) {
      // TODO: Set error
      return 0;
    }
  }

  // Create and populate hive_set_value structure
  // TODO: From the hivex docs
  // Note that the value field is just treated as a list of bytes, and is stored
  // directly in the hive. The caller has to ensure correct encoding and
  // endianness, for example converting dwords to little endian.
  hive_set_value key_val;
  key_val.key=(char*)malloc((sizeof(char)*key_name.toAscii().count())+1);
  key_val.value=(char*)malloc(sizeof(char)*key_value.size());
  if(key_val.key==NULL || key_val.value==NULL) {
    // TODO: Set error
    return 0;
  }
  strcpy(key_val.key,key_name.toAscii().constData());
  key_val.t=(hive_type)this->StringToKeyValueType(key_value_type);
  key_val.len=key_value.size();
  memcpy(key_val.value,key_value.constData(),key_value.size());

  // Create/Update key
  if(hivex_node_set_value(this->p_hive,parent_node,&key_val,0)!=0) {
    // TODO: Set error
    return 0;
  }

  // Free the hive_set_value structure
  free(key_val.key);
  free(key_val.value);

  // To make sure everything worked, a hadle to the new key is now requeried
  // from hive and then returned
  hive_value_h key;
  if(!this->GetKeyHandle(parent_node_path,key_name,&key)) {
    // TODO: Set error
    return 0;
  }

  this->has_changes_to_commit=true;
  return key;
}
