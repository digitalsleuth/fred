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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QSplitter>

#include <QString>
#include <QByteArray>

#include <hivex.h>

#include "registryhive.h"
#include "registrynodetree.h"
#include "registrynodetreemodel.h"
#include "registrykeytable.h"
#include "registrykeytablemodel.h"
#include "qhexedit/qhexedit.h"
#include "datainterpreter.h"
#include "datareporter.h"
#include "threadsearch.h"
#include "searchresultwidget.h"
#include "tabwidget.h"

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
    void on_ActionSearch_triggered();

    void SlotNodeTreeClicked(QModelIndex index);
    void SlotKeyTableClicked(QModelIndex index);
    void SlotKeyTableDoubleClicked(QModelIndex index);
    void SlotHexEditAddressChanged(int hex_offset);
    void SlotReportClicked();
    void SlotSearchFinished();
    void SlotSearchResultWidgetDoubleClicked(QModelIndex index);
    void SlotTabCloseButtonClicked(int index);

private:
    Ui::MainWindow *ui;
    QString last_open_location;
    RegistryHive *p_hive;
    bool is_hive_open;
    RegistryNodeTreeModel *p_reg_node_tree_model;
    RegistryKeyTableModel *p_reg_key_table_model;
    QByteArray selected_key_value;
    QList<SearchResultWidget*> search_result_widgets;

    // Widgets etc...
    RegistryNodeTree *p_node_tree;
    RegistryKeyTable *p_key_table;
    TabWidget *p_tab_widget;
    QWidget *p_hex_edit_widget;
    QHexEdit *p_hex_edit;
    QLabel *p_hex_edit_status_bar;
    DataInterpreter *p_data_interpreter;
    QVBoxLayout *p_hex_edit_layout;
    QSplitter *p_horizontal_splitter;
    QSplitter *p_horizontal_splitter2;
    QSplitter *p_vertical_splitter;
    DataReporter *p_data_reporter;
    ThreadSearch *p_search_thread;

    /*
     * CheckUserConfigDir
     *
     * Checks for and possibly creates the ~/.fred directory
     */
    void CheckUserConfigDir();
    /*
     * UpdateWindowTitle
     *
     * Updates the window title
     */
    void UpdateWindowTitle(QString filename="");
    /*
     * UpdateDataInterpreter
     *
     * Update data interpreter
     */
    void UpdateDataInterpreter(int hex_offset);
    /*
     * UpdateDataReporterMenu
     *
     */
    void UpdateDataReporterMenu();
    /*
     * OpenHive
     *
     * Open a registry hive
     */
    void OpenHive(QString hive_file);
    /*
     * ParseCommandLineArgs
     *
     * Parse command line arguments
     */
    void ParseCommandLineArgs();
};

#endif // MAINWINDOW_H
