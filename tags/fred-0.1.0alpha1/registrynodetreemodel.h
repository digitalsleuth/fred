/*******************************************************************************
* fred Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>             *
*                                                                              *
* Forensic Registry EDitor (fred) is a M$ registry hive editor for Linux with  *
* special feautures useful during forensic analysis.                           *
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

#ifndef REGISTRYNODETREEMODEL_H
#define REGISTRYNODETREEMODEL_H

#include <QAbstractItemModel>

#include "registrynode.h"

#include <hivex.h>

class RegistryNodeTreeModel : public QAbstractItemModel {
  Q_OBJECT

  public:
    RegistryNodeTreeModel(hive_h *hhive,
                          hive_node_h hive_root_node,
                          QObject *p_parent=0);
    ~RegistryNodeTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role=Qt::DisplayRole) const;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent=QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;

  private:
    void SetupModelData(hive_h *hhive,
                        hive_node_h hive_node,
                        RegistryNode *p_parent);
    RegistryNode *p_root_node;
};

#endif // REGISTRYNODETREEMODEL_H
