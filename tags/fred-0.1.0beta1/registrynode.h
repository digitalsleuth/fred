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

#ifndef REGISTRYNODE_H
#define REGISTRYNODE_H

#include <QList>
#include <QString>

#include <inttypes.h>

class RegistryNode {
  public:
    RegistryNode(const QString name, RegistryNode *p_parent=0);
    ~RegistryNode();

    void AppendChild(RegistryNode *p_child);
    RegistryNode *child(uint64_t row);
    uint64_t childCount() const;
    QString data() const;
    uint64_t row() const;
    RegistryNode *parent();

  private:
    QList<RegistryNode*> child_nodes;
    QString node_name;
    RegistryNode *p_parent_node;
};

#endif // REGISTRYNODE_H
