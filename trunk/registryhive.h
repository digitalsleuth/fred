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

#ifndef REGISTRYHIVE_H
#define REGISTRYHIVE_H

#include <QObject>
#include <QMap>

#include <hivex.h>

class RegistryHive : public QObject {
  Q_OBJECT

  public:
    typedef struct sRegistryKey {
      QString name;
      int type;
      size_t value_len;
      QByteArray value;
    } tsRegistryKey, *ptsRegistryKey;

    typedef enum eHiveType {
      eHiveType_UNKNOWN=0,
      eHiveType_SYSTEM,
      eHiveType_SOFTWARE,
      eHiveType_SAM,
      eHiveType_SECURITY,
      eHiveType_NTUSER
    } teHiveType;

    explicit RegistryHive(QObject *p_parent=0);
    ~RegistryHive();

    bool Error();
    QString GetErrorMsg();

    bool Open(QString file, bool read_only=true);
    bool Reopen(bool read_only=true);
    bool CommitChanges();
    bool Close();

    QString Filename();
    teHiveType HiveType();
    QString HiveTypeToString(teHiveType hive_type);
    bool HasChangesToCommit();

    QMap<QString,int> GetNodes(QString path="\\");
    QMap<QString,int> GetNodes(int parent_node=0);
    QMap<QString,int> GetKeys(QString path="\\");
    QMap<QString,int> GetKeys(int parent_node=0);
    QByteArray GetKeyValue(QString path,
                           QString key,
                           int *p_value_type,
                           size_t *p_value_len);
    QByteArray GetKeyValue(int hive_key,
                           int *p_value_type,
                           size_t *p_value_len);
    int64_t GetNodeModTime(QString path);
    int64_t GetNodeModTime(int node);
    static QString KeyValueToString(QByteArray value, int value_type);
    static QString KeyValueToString(QByteArray value,
                                    QString format,
                                    int offset=0,
                                    int length=0,
                                    bool little_endian=true);
    static QStringList KeyValueToStringList(QByteArray value, int value_type);
    static QStringList GetKeyValueTypes();
    // TODO: Rename KeyTypeToString to KeyValueTypeToString
    static QString KeyTypeToString(int value_type);
    static uint64_t FiletimeToUnixtime(int64_t filetime);

    int AddNode(QString parent_node_path, QString node_name);
    bool DeleteNode(QString node_path);

    bool AddKey(QString parent_node_path,
                QString key_name,
                QString key_type,
                QByteArray key_value,
                ptsRegistryKey resulting_key);
    bool UpdateKey(QString parent_node_path,
                   QString key_name,
                   QString key_type,
                   QByteArray key_value,
                   ptsRegistryKey resulting_key);
    bool DeleteKey(QString parent_node_path, QString key_name);

  private:
    QString erro_msg;
    bool is_error;
    QString hive_file;
    hive_h *p_hive;
    bool is_hive_open;
    bool is_hive_writable;
    bool has_changes_to_commit;

    void SetError(QString msg);
    bool GetNodeHandle(QString &path, hive_node_h *p_node);
    QMap<QString,int> GetNodesHelper(hive_node_h parent_node);
    QMap<QString,int> GetKeysHelper(hive_node_h parent_node);
    QByteArray GetKeyValueHelper(hive_value_h hive_key,
                                 int *p_value_type,
                                 size_t *p_value_len);
    bool PathExists(QString path);
    int StringToKeyValueType(QString value_type);
    bool GetKey(QString &parent_node_path,
                QString &key_name,
                ptsRegistryKey *key_value);
    bool GetKeys(QString &parent_node_path,
                 QList<ptsRegistryKey> *p_key_values);
    bool SetKey(QString &parent_node_path,
                QString &key_name,
                QString &key_type,
                QByteArray &key_value,
                ptsRegistryKey *resulting_key,
                bool create_key);

};

#endif // REGISTRYHIVE_H
