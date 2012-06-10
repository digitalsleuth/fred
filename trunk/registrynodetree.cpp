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

#include "registrynodetree.h"
#include "registrynodetreemodel.h"

#include <QHeaderView>
#include <QApplication>
#include <QClipboard>

RegistryNodeTree::RegistryNodeTree(QWidget *p_parent) : QTreeView(p_parent) {
  // Configure widget
  this->setTextElideMode(Qt::ElideNone);

  // Create context menu
  this->p_menu_copy=new QMenu(tr("Copy"),this);
  this->p_action_copy_node_name=new QAction(tr("Copy node name"),
                                            this->p_menu_copy);
  this->p_menu_copy->addAction(this->p_action_copy_node_name);
  this->connect(this->p_action_copy_node_name,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyNodeName()));
  this->p_action_copy_node_path=new QAction(tr("Copy node path"),
                                            this->p_menu_copy);
  this->p_menu_copy->addAction(this->p_action_copy_node_path);
  this->connect(this->p_action_copy_node_path,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyNodePath()));
}

RegistryNodeTree::~RegistryNodeTree() {
  // Delete context menu
  delete this->p_action_copy_node_name;
  delete this->p_action_copy_node_path;
  delete this->p_menu_copy;
}

void RegistryNodeTree::setModel(QAbstractItemModel *p_model) {
  QTreeView::setModel(p_model);
  this->header()->setResizeMode(0,QHeaderView::ResizeToContents);
  this->header()->setStretchLastSection(false);
  if(p_model!=NULL && p_model->index(0,0).isValid()) {
    // Select first tree item
    this->setCurrentIndex(p_model->index(0,0));
  }
}

//int RegistryNodeTree::sizeHintForColumn(int column) const {}

void RegistryNodeTree::contextMenuEvent(QContextMenuEvent *p_event) {
  // Only show context menu when a node is selected
  if(this->selectedIndexes().count()!=1) return;
  // Only show context menu when user clicked on selected row
  if(this->indexAt(p_event->pos())!=this->selectedIndexes().at(0)) return;

  // Emit a click signal
  emit(this->clicked(this->indexAt(p_event->pos())));

  // Create context menu and add actions
  QMenu context_menu(this);
  context_menu.addMenu(this->p_menu_copy);
  context_menu.exec(p_event->globalPos());
}

void RegistryNodeTree::keyPressEvent(QKeyEvent *p_event) {
  // Only react if a node is selected and user pressed Key_Left
  if(this->selectedIndexes().count()==1 &&
     p_event->key()==Qt::Key_Left)
  {
    QModelIndex cur_index=this->selectedIndexes().at(0);

    if(this->model()->hasChildren(cur_index)) {
      // Collapse current node
      this->collapse(cur_index);
    }
    if(!cur_index.parent().isValid()) {
      // Do no try to collapse anything above root node
      return;
    }
    this->collapse(cur_index.parent());
    this->setCurrentIndex(cur_index.parent());
    return;
  }

  // If we didn't handle the key event, let our parent handle it
  QTreeView::keyPressEvent(p_event);
}

void RegistryNodeTree::currentChanged(const QModelIndex &current,
                                      const QModelIndex &previous)
{
  // Call parent class's currentChanged first
  QTreeView::currentChanged(current,previous);

  // Now emit our signal
  QModelIndex current_item=QModelIndex(current);
  emit(RegistryNodeTree::CurrentItemChanged(current_item));
}

void RegistryNodeTree::SlotCopyNodeName() {
  QApplication::clipboard()->
    setText(this->selectedIndexes().at(0).data().toString(),
            QClipboard::Clipboard);
}

void RegistryNodeTree::SlotCopyNodePath() {
  QString path=((RegistryNodeTreeModel*)(this->model()))->
    GetNodePath(this->selectedIndexes().at(0));
  QApplication::clipboard()->setText(path,QClipboard::Clipboard);
}
