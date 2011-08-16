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

#ifndef REGISTRYHIVE_H
#define REGISTRYHIVE_H

#include <QObject>
#include <QMap>

#include <hivex.h>

class RegistryHive : public QObject {
  Q_OBJECT

  public:
    explicit RegistryHive(QObject *p_parent=0);
    ~RegistryHive();

    bool Error();
    QString GetErrorMsg();

    bool Open(QString file, bool read_only=true);
    bool Close(bool commit_changes=false);

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
    Q_INVOKABLE static QString KeyValueToString(QByteArray value,
                                                int value_type);
    Q_INVOKABLE static QString KeyTypeToString(int value_type);

  private:
    QString erro_msg;
    bool is_error;
    QString hive_file;
    hive_h *p_hive;
    bool is_hive_open;

    void SetError(QString msg);
    bool GetNodeHandle(QString &path, hive_node_h *p_node);
    QMap<QString,int> GetNodesHelper(hive_node_h parent_node);
    QMap<QString,int> GetKeysHelper(hive_node_h parent_node);
    QByteArray GetKeyValueHelper(hive_value_h hive_key,
                                 int *p_value_type,
                                 size_t *p_value_len);

};

#endif // REGISTRYHIVE_H
