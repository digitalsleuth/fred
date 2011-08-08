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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <hivex.h>

#include "registrynodetreemodel.h"
#include "registrykeytablemodel.h"
#include "qhexedit/qhexedit.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  private slots:
    void on_action_Quit_triggered();
    void on_action_Open_hive_triggered();
    void on_action_Close_hive_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionAbout_fred_triggered();

    void SlotNodeTreeClicked(QModelIndex index);
    void SlotKeyTableClicked(QModelIndex index);
    void SlotKeyTableDoubleClicked(QModelIndex index);

private:
    Ui::MainWindow *ui;
    QString last_open_location;
    hive_h *hhive;
    bool is_hive_open;
    RegistryNodeTreeModel *p_reg_node_tree_model;
    RegistryKeyTableModel *p_reg_key_table_model;
    QByteArray selected_key_value;

    // Widgets
    QTreeView *p_node_tree;
    QTableView *p_key_table;
    QHexEdit *p_hex_edit;
    QSplitter *p_horizontal_splitter;
    QSplitter *p_vertical_splitter;

    void UpdateWindowTitle(QString filename="");
};

#endif // MAINWINDOW_H
