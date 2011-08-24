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

#include "threadsearch.h"

#include <QStringList>

ThreadSearch::ThreadSearch(QObject *p_parent) : QThread(p_parent) {
  this->hive_file="";
  this->h_hive=NULL;
  this->keywords=QList<QByteArray>();
  this->search_nodes=false;
  this->search_keys=false;
  this->search_values=false;
  this->root_node=0;
}

bool ThreadSearch::Search(QString registry_hive,
                          QList<QByteArray> search_keywords,
                          bool search_node_names,
                          bool search_key_names,
                          bool search_key_values,
                          QString search_path)
{
  this->hive_file=registry_hive;
  this->keywords=search_keywords;
  this->search_nodes=search_node_names;
  this->search_keys=search_key_names;
  this->search_values=search_key_values;
  this->root_path=search_path;

  // Try to open hive
  this->h_hive=hivex_open(this->hive_file.toAscii().constData(),0);
  if(this->h_hive==NULL) return false;

  // Get root node
  this->root_node=hivex_root(this->h_hive);
  if(this->root_node==0) {
    hivex_close(this->h_hive);
    return false;
  }

  // If a root path was specified, iterate to it
  if(search_path!="\\") {
    QStringList path_nodes=search_path.split("\\",QString::SkipEmptyParts);
    int i;
    for(i=0;i<path_nodes.count();i++) {
      this->root_node=hivex_node_get_child(this->h_hive,
                                           this->root_node,
                                           path_nodes.at(i).toAscii().constData());
      if(this->root_node==0) {
        hivex_close(this->h_hive);
        return false;
      }
    }
  }

  this->start();
  return true;
}

void ThreadSearch::run() {
  qDebug("Starting search");
  this->Match(this->root_node,this->root_path=="\\" ? "" : this->root_path);
  qDebug("Ending search");
  hivex_close(this->h_hive);
}

void ThreadSearch::Match(hive_node_h node, QString path) {
  char *p_node_name;
  int i;
  hive_node_h *p_node_childs;
  QByteArray *p_byte_array;

  //qDebug("Searching in '%s'",path.toAscii().constData());

  p_node_name=hivex_node_name(this->h_hive,node);
  if(p_node_name==NULL) return;

  if(this->search_nodes) {
    // Compare node name with keywords
    p_byte_array=new QByteArray(p_node_name);
    for(i=0;i<this->keywords.count();i++) {
      if(p_byte_array->indexOf(this->keywords.at(i))!=-1) {
        emit(SIGNAL(SignalFoundMatch(ThreadSearch::eMatchType_NodeName,
                                     path,
                                     QString(p_node_name),
                                     QString())));
      }
    }
    delete p_byte_array;
  }

  p_node_childs=hivex_node_children(this->h_hive,node);
  if(p_node_childs!=NULL) {
    for(i=0;p_node_childs[i];i++) {
      this->Match(p_node_childs[i],
                  QString(path).append("\\").append(p_node_name));
    }
    delete p_node_childs;
  }
  delete p_node_name;
}
