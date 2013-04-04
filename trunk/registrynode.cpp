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

#include "registrynode.h"

RegistryNode::RegistryNode(const QList<QVariant> &data,
                           RegistryNode *p_parent)
{
  this->node_data=data;
  this->p_parent_node=p_parent;
}

RegistryNode::~RegistryNode() {
  qDeleteAll(this->child_nodes);
}

void RegistryNode::AppendChild(RegistryNode *p_child) {
  this->child_nodes.append(p_child);
}

RegistryNode* RegistryNode::Child(uint64_t row) {
  return this->child_nodes.value(row);
}

uint64_t RegistryNode::ChildCount() const {
  return this->child_nodes.count();
}

QVariant RegistryNode::Data(int column) const {
  if(column>=0 && column<2) {
    return this->node_data.value(column);
  } else {
    return QVariant();
  }
}

uint64_t RegistryNode::Row() const {
  if(this->p_parent_node) {
    return this->p_parent_node->
      child_nodes.indexOf(const_cast<RegistryNode*>(this));
  } else {
    return 0;
  }
}

RegistryNode *RegistryNode::Parent() {
  return this->p_parent_node;
}
