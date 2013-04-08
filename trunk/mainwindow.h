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

#include "argparser.h"
#include "registryhive.h"
#include "registrynodetree.h"
#include "registrynodetreemodel.h"
#include "registrynodetreemodelproxy.h"
#include "registrykeytable.h"
#include "registrykeytablemodel.h"
#include "hexeditwidget.h"
#include "reports.h"
#include "threadsearch.h"
#include "searchresultwidget.h"
#include "tabwidget.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit MainWindow(ArgParser *p_arg_parser);
    ~MainWindow();

  private slots:
    void on_action_Quit_triggered();
    void on_action_Open_hive_triggered();
    void on_action_Close_hive_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionAbout_fred_triggered();
    void on_ActionSearch_triggered();
    void on_ActionGenerateReport_triggered();

    void SlotNodeTreeClicked(QModelIndex index);
    void SlotKeyTableClicked(QModelIndex index);
    void SlotKeyTableDoubleClicked(QModelIndex index);
    void SlotSearchFinished();
    void SlotSearchResultWidgetDoubleClicked(QModelIndex index);
    void SlotTabCloseButtonClicked(int index);

    void on_ActionReloadReportTemplates_triggered();

  private:
    Ui::MainWindow *ui;
    ArgParser *p_args;
    QString last_open_location;
    RegistryHive *p_hive;
    bool is_hive_open;
    QByteArray selected_key_value;
    QList<SearchResultWidget*> search_result_widgets;

    // Models
    RegistryNodeTreeModel *p_reg_node_tree_model;
    RegistryNodeTreeModelProxy *p_reg_node_tree_model_proxy;
    RegistryKeyTableModel *p_reg_key_table_model;

    // Widgets etc...
    RegistryNodeTree *p_node_tree;
    RegistryKeyTable *p_key_table;
    TabWidget *p_tab_widget;
    HexEditWidget *p_hex_edit_widget;
    QSplitter *p_horizontal_splitter;
    QSplitter *p_vertical_splitter;
    Reports *p_reports;

    // Threads
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
     * OpenHive
     *
     * Open a registry hive
     */
    void OpenHive(QString hive_file);
    void ReloadReportTemplates();
};

#endif // MAINWINDOW_H
