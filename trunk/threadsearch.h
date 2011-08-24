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

#ifndef THREADSEARCH_H
#define THREADSEARCH_H

#include <QThread>
#include <QObject>
#include <QList>
#include <QByteArray>

#include <hivex.h>

class ThreadSearch : public QThread {
  Q_OBJECT

  public:
    ThreadSearch(QObject *p_parent=0);

    bool Search(QString registry_hive,
                QList<QByteArray> search_keywords,
                bool search_node_names,
                bool search_key_names,
                bool search_key_values,
                QString search_path="\\");

  protected:
    void run();

  private:
    QString hive_file;
    hive_h *h_hive;
    QList<QByteArray> keywords;
    bool search_nodes;
    bool search_keys;
    bool search_values;
    hive_node_h root_node;


};

#endif // THREADSEARCH_H
