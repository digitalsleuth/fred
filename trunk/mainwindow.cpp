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

#ifndef FRED_REPORT_TEMPLATE_DIR
  #ifndef __MINGW32__
    #define FRED_REPORT_TEMPLATE_DIR "/usr/share/fred/report_templates/"
  #else
    #define FRED_REPORT_TEMPLATE_DIR ".\\report_templates\\"
  #endif
#endif

#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QDesktopWidget>
#include <QDir>
#include <QSplitter>
#include <QListIterator>
#include <QInputDialog>

#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlgabout.h"
#include "dlgkeydetails.h"
#include "dlgreportchooser.h"
#include "dlgreportviewer.h"
#include "dlgsearch.h"
#include "dlgpreferences.h"

#include "compileinfo.h"

/*******************************************************************************
 * Public
 ******************************************************************************/

/*
 * Constructor
 */
MainWindow::MainWindow(ArgParser *p_arg_parser) :
  QMainWindow(0), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Initialize private vars
  this->p_args=p_arg_parser;
  this->p_hive=new RegistryHive(this);
  this->is_hive_open=false;
  this->p_reg_node_tree_model=NULL;
  this->p_reg_node_tree_model_proxy=NULL;
  this->p_reg_key_table_model=NULL;
  this->p_search_thread=NULL;
  this->search_result_widgets.clear();

  // Init and load settings
  this->p_settings=new Settings(this);
  this->is_hive_writable=!this->p_settings->GetOpenHivesReadOnly();

  // Set main window size
  QByteArray geometry=this->p_settings->GetWindowGeometry("MainWindow");
  if(!geometry.isEmpty()) {
    // Restore saved geometry
    this->restoreGeometry(geometry);
  } else {
    // No saved geometry, calculate and set default
    int cur_screen=QApplication::desktop()->screenNumber(this);
    int window_width=
      QApplication::desktop()->availableGeometry(cur_screen).width()*0.5;
    int window_height=
      QApplication::desktop()->availableGeometry(cur_screen).height()*0.5;
    int window_x=
      (QApplication::desktop()->availableGeometry(cur_screen).width()/2)-
      (window_width/2);
    int window_y=
      (QApplication::desktop()->availableGeometry(cur_screen).height()/2)-
      (window_height/2);
    this->setGeometry(window_x,
                      window_y,
                      window_width,
                      window_height);
  }

  // Create widgets
  this->p_horizontal_splitter=new QSplitter();
  this->p_horizontal_splitter->setOrientation(Qt::Horizontal);

  this->p_node_tree=new RegistryNodeTree(this->p_horizontal_splitter);

  this->p_vertical_splitter=new QSplitter(this->p_horizontal_splitter);
  this->p_vertical_splitter->setOrientation(Qt::Vertical);

  this->p_key_table=new RegistryKeyTable(this->p_vertical_splitter);

  this->p_tab_widget=new TabWidget(this->p_vertical_splitter);

  this->p_hex_edit_widget=new HexEditWidget();

  // Add hexedit page to tab_widget
  this->p_tab_widget->addTab(this->p_hex_edit_widget,tr("Hex viewer"));

  // Add widgets to their splitters
  this->p_vertical_splitter->addWidget(this->p_key_table);
  this->p_vertical_splitter->addWidget(this->p_tab_widget);

  this->p_horizontal_splitter->addWidget(this->p_node_tree);
  this->p_horizontal_splitter->addWidget(this->p_vertical_splitter);

  // Set stretch factors
  QSizePolicy node_tree_policy=this->p_node_tree->sizePolicy();
  node_tree_policy.setHorizontalStretch(1);
  node_tree_policy.setVerticalStretch(100);
  this->p_node_tree->setSizePolicy(node_tree_policy);

  QSizePolicy vertical_splitter_policy=this->p_vertical_splitter->sizePolicy();
  vertical_splitter_policy.setHorizontalStretch(4);
  vertical_splitter_policy.setVerticalStretch(100);
  this->p_vertical_splitter->setSizePolicy(vertical_splitter_policy);

  QSizePolicy key_table_policy=this->p_key_table->sizePolicy();
  key_table_policy.setVerticalStretch(5);
  key_table_policy.setHorizontalStretch(100);
  this->p_key_table->setSizePolicy(key_table_policy);

  QSizePolicy tab_widget_policy=this->p_tab_widget->sizePolicy();
  tab_widget_policy.setVerticalStretch(2);
  tab_widget_policy.setHorizontalStretch(200);
  this->p_tab_widget->setSizePolicy(tab_widget_policy);

  // Connect signals
  this->connect(this->p_node_tree,
                SIGNAL(clicked(QModelIndex)),
                this,
                SLOT(SlotNodeTreeClicked(QModelIndex)));
  this->connect(this->p_node_tree,
                SIGNAL(activated(QModelIndex)),
                this,
                SLOT(SlotNodeTreeClicked(QModelIndex)));
  this->connect(this->p_node_tree,
                SIGNAL(CurrentItemChanged(QModelIndex)),
                this,
                SLOT(SlotNodeTreeClicked(QModelIndex)));
  this->connect(this->p_key_table,
                SIGNAL(clicked(QModelIndex)),
                this,
                SLOT(SlotKeyTableClicked(QModelIndex)));
  this->connect(this->p_key_table,
                SIGNAL(doubleClicked(QModelIndex)),
                this,
                SLOT(SlotKeyTableDoubleClicked(QModelIndex)));
  this->connect(this->p_key_table,
                SIGNAL(CurrentItemChanged(QModelIndex)),
                this,
                SLOT(SlotKeyTableClicked(QModelIndex)));
  this->connect(this->p_tab_widget,
                SIGNAL(tabCloseRequested(int)),
                this,
                SLOT(SlotTabCloseButtonClicked(int)));
  this->connect(this->p_node_tree,
                SIGNAL(SignalAddNode(QModelIndex)),
                this,
                SLOT(SlotAddNode(QModelIndex)));
  this->connect(this->p_node_tree,
                SIGNAL(SignalDeleteNode(QModelIndex)),
                this,
                SLOT(SlotDeleteNode(QModelIndex)));

  // Add central widget
  this->setCentralWidget(this->p_horizontal_splitter);
  this->centralWidget()->setContentsMargins(4,4,4,0);

  // Set window title
  this->UpdateWindowTitle();

  // Create and update recently opened menu
  this->p_recently_opened_menu=new QMenu(this);
  this->ui->ActionRecentlyOpened->setMenu(this->p_recently_opened_menu);
  this->UpdateRecentlyOpenedMenu();

  // Update EnableWriteSupport menu according to defaults
  this->UpdateEnableWriteSupportMenu();

  // Load report templates
  this->p_reports=new Reports(this->p_settings);

  // Finally, react on some command line arguments
  if(this->p_args->IsSet("maximized")) {
    this->setWindowState(Qt::WindowMaximized);
  }
  if(this->p_args->IsSet("fullscreen")) {
    this->setWindowState(Qt::WindowFullScreen);
  }
  if(this->p_args->IsSet("hive-file")) {
    this->OpenHive(this->p_args->GetArgVal("hive-file"));
  }
}

/*
 * Destructor
 */
MainWindow::~MainWindow() {
  if(this->is_hive_open) {
    this->p_hive->Close();
  }

  // Delete created objects
  delete this->p_reports;
  this->ClearRecentlyOpenedMenu();
  delete this->p_recently_opened_menu;
  delete this->p_hex_edit_widget;
  delete this->p_tab_widget;
  delete this->p_key_table;
  delete this->p_vertical_splitter;
  delete this->p_node_tree;
  delete this->p_horizontal_splitter;
  delete this->p_settings;
  delete this->p_hive;
  delete ui;
}

/*******************************************************************************
 * Protected
 ******************************************************************************/

/*
 * closeEvent
 */
void MainWindow::closeEvent(QCloseEvent *p_event) {
  Q_UNUSED(p_event)

  // Make sure the user can save any changes
  // TODO: If saving fails, let the user cancel closing
  this->SaveHiveChanges();

  // Save window position and size on exit
  this->p_settings->SetWindowGeometry("MainWindow",this->saveGeometry());
  QMainWindow::closeEvent(p_event);
}

/*******************************************************************************
 * Private slots
 ******************************************************************************/

/*
 * on_action_Open_hive_triggered
 */
void MainWindow::on_action_Open_hive_triggered() {
  QString hive_file="";

  hive_file=QFileDialog::getOpenFileName(this,
                                    tr("Open registry hive"),
                                    this->p_settings->GetLastOpenLocation(),
                                    tr("All files (*)"));
  if(hive_file=="") return;

  this->OpenHive(hive_file);
}

/*
 * on_action_Close_hive_triggered
 */
void MainWindow::on_action_Close_hive_triggered() {
  // Make sure the user can save any changes
  // TODO: If saving fails, let the user cancel closing
  this->SaveHiveChanges();

  if(this->is_hive_open) {
    // Remove search results
    while(this->p_tab_widget->count()>1) {
      this->p_tab_widget->removeTab(this->p_tab_widget->count()-1);
      delete this->search_result_widgets.at(this->p_tab_widget->count()-1);
      this->search_result_widgets.removeLast();
    }

    // Delete models
    if(this->p_reg_node_tree_model!=NULL) {
      this->p_node_tree->setModel(NULL);
      delete this->p_reg_node_tree_model_proxy;
      delete this->p_reg_node_tree_model;
      this->p_reg_node_tree_model_proxy=NULL;
      this->p_reg_node_tree_model=NULL;
    }
    if(this->p_reg_key_table_model!=NULL) {
      this->p_key_table->setModel(NULL);
      delete this->p_reg_key_table_model;
      this->p_reg_key_table_model=NULL;
    }

    // Remove any data from hex edit and data interpreter
    this->p_hex_edit_widget->SetData(QByteArray());
    this->p_hex_edit_widget->setEnabled(false);

    // Close hive
    this->p_hive->Close();

    this->is_hive_open=false;
    this->is_hive_writable=!this->p_settings->GetOpenHivesReadOnly();
    this->UpdateWindowTitle();
    this->UpdateMenuStates();
    this->UpdateEnableWriteSupportMenu();
  }
}

/*
 * on_action_Quit_triggered
 */
void MainWindow::on_action_Quit_triggered() {
  qApp->exit();
}

/*
 * on_ActionSearch_triggered
 */
void MainWindow::on_ActionSearch_triggered() {
  DlgSearch dlg_search(this);
  if(dlg_search.exec()==QDialog::Accepted) {
    // Create search thread and connect needed signals/slots
    this->p_search_thread=new ThreadSearch(this);

    // Add new search widget to tabwidget and to internal widget list
    SearchResultWidget *p_search_widget=
      new SearchResultWidget(this->p_tab_widget);
    p_search_widget->setEnabled(false);
    this->search_result_widgets.append(p_search_widget);

    this->connect(p_search_widget,
                  SIGNAL(doubleClicked(QModelIndex)),
                  this,
                  SLOT(SlotSearchResultWidgetDoubleClicked(QModelIndex)));

    this->p_tab_widget->addTab(p_search_widget,tr("Search results"),true);
    this->p_tab_widget->setCurrentIndex(this->p_tab_widget->count()-1);

    // Connect search thread to result widget
    this->connect(this->p_search_thread,
                  SIGNAL(SignalFoundMatch(ThreadSearch::eMatchType,
                                          QString,QString,QString)),
                  p_search_widget,
                  SLOT(SlotFoundMatch(ThreadSearch::eMatchType,
                                      QString,QString,QString)));
    this->connect(this->p_search_thread,
                  SIGNAL(finished()),
                  this,
                  SLOT(SlotSearchFinished()));
    this->connect(this->p_search_thread,
                  SIGNAL(finished()),
                  p_search_widget,
                  SLOT(SlotSearchFinished()));

    // Start searching
    this->ui->ActionSearch->setEnabled(false);
    p_search_thread->Search(this->p_hive->Filename(),
                            dlg_search.Keywords(),
                            dlg_search.SearchNodeNames(),
                            dlg_search.SearchKeyNames(),
                            dlg_search.SearchKeyValues());
  }
}

/*
 * on_ActionEnableWriteSupport_triggered
 */
void MainWindow::on_ActionEnableWriteSupport_triggered() {
  // There might be unsaved changes, give the user the chance to save them
  // TODO: If saving failed, give the user the chance to cancel switching
  this->SaveHiveChanges();

  // Switch write support
  this->is_hive_writable=!this->is_hive_writable;
  this->UpdateEnableWriteSupportMenu();
  this->p_node_tree->SetWritable(this->is_hive_writable);
  this->p_key_table->SetWritable(this->is_hive_writable);
  this->UpdateWindowTitle(this->p_hive->Filename());
}

/*
 * on_ActionPreferences_triggered
 */
void MainWindow::on_ActionPreferences_triggered() {
  DlgPreferences dlg_preferences(this->p_settings,this);
  dlg_preferences.exec();
  // Update vars, objects and GUI elements which might be affected by the new
  // settings
  this->UpdateRecentlyOpenedMenu();
  this->is_hive_writable=!this->p_settings->GetOpenHivesReadOnly();
  this->UpdateEnableWriteSupportMenu();
  this->p_reports->LoadReportTemplates();
}

/*
 * on_ActionGenerateReport_triggered
 */
void MainWindow::on_ActionGenerateReport_triggered() {
  DlgReportChooser dlg_repchooser(this->p_reports,
                                  this->p_hive->HiveTypeToString(
                                    this->p_hive->HiveType()),
                                  this->p_settings,
                                  this);
  if(dlg_repchooser.exec()==QDialog::Accepted) {
    QList<ReportTemplate*> selected_reports;

    // Get selected report
    selected_reports=dlg_repchooser.GetSelectedReports();
    if(selected_reports.isEmpty()) return;

    // Generate report(s)
    QString report_result="";
    if(this->p_reports->GenerateReport(this->p_hive,
                                       selected_reports,
                                       report_result,
                                       false))
    {
      // Report generation was successfull, show reports
      DlgReportViewer *p_dlg_report_view=new DlgReportViewer(report_result,
                                                             this->p_settings,
                                                             this);
      p_dlg_report_view->exec();
      delete p_dlg_report_view;
    } else {
      // TODO: Inform user
      qDebug()<<"ERROR: "<<report_result;
    }
  }
}

/*
 * on_ActionReloadReportTemplates_triggered
 */
void MainWindow::on_ActionReloadReportTemplates_triggered() {
  this->p_reports->LoadReportTemplates();
}

/*
 * on_actionAbout_Qt_triggered
 */
void MainWindow::on_actionAbout_Qt_triggered() {
  QMessageBox::aboutQt(this,tr("About Qt"));
}

/*
 * on_actionAbout_fred_triggered
 */
void MainWindow::on_actionAbout_fred_triggered() {
  DlgAbout dlg_about(this);
  dlg_about.exec();
}

/*
 * SlotNodeTreeClicked
 */
void MainWindow::SlotNodeTreeClicked(QModelIndex index) {
  QString node_path;

  if(!index.isValid()) return;

  // Map proxy index to tree model index
  index=this->p_reg_node_tree_model_proxy->mapToSource(index);

  // Built node path
  node_path=this->p_reg_node_tree_model->GetNodePath(index);

  // Create table model and attach it to the table view
  if(this->p_reg_key_table_model!=NULL) {
    // If a previous model was set, delete it and clear hexedit etc...
    this->p_key_table->setModel(NULL);
    delete this->p_reg_key_table_model;
    this->p_hex_edit_widget->SetData(QByteArray());
  }
  this->p_reg_key_table_model=new RegistryKeyTableModel(this->p_hive,node_path);
  this->p_key_table->setModel(this->p_reg_key_table_model,
                              this->is_hive_writable);
  // Set focus back to nodetree to be able to navigate with keyboard
  this->p_node_tree->setFocus();
}

/*
 * SlotKeyTableClicked
 */
void MainWindow::SlotKeyTableClicked(QModelIndex index) {
  if(!index.isValid()) return;

  this->selected_key_value=
    this->p_reg_key_table_model->data(this->p_reg_key_table_model->
                                        index(index.row(),2),
                                      RegistryKeyTableModel::
                                        AdditionalRoles_GetRawData)
                                          .toByteArray();
  this->p_hex_edit_widget->SetData(this->selected_key_value);
  // Set focus back to nodetree to be able to navigate with keyboard
  this->p_key_table->setFocus();
}

/*
 * SlotKeyTableDoubleClicked
 */
void MainWindow::SlotKeyTableDoubleClicked(QModelIndex index) {
  Q_UNUSED(index);
  /*
  QModelIndex key_index;
  QModelIndex node_index;
  QStringList nodes;

  QString key_name;
  QString key_type;
  QByteArray key_value;

  if(!index.isValid()) return;

  // Get key name, type and value
  key_index=this->p_reg_key_table_model->index(index.row(),0);
  key_name=this->p_reg_key_table_model->data(key_index,Qt::DisplayRole)
    .toString();
  key_index=this->p_reg_key_table_model->index(index.row(),1);
  key_type=this->p_reg_key_table_model->data(key_index,Qt::DisplayRole)
    .toString();ThreadSearch
  key_index=this->p_reg_key_table_model->index(index.row(),2);
  key_value=this->p_reg_key_table_model->data(key_index,
                                              RegistryKeyTableModel::
                                                AdditionalRoles_GetRawData)
    .toByteArray();

  // Get current node
  node_index=this->p_node_tree->currentIndex();

  //Built node path
  nodes.clear();
  nodes.append(this->p_reg_node_tree_model->
                 data(node_index,Qt::DisplayRole).toString());
  while(this->p_reg_node_tree_model->parent(node_index)!=QModelIndex()) {
    // Prepend all parent nodes
    node_index=this->p_reg_node_tree_model->parent(node_index);
    nodes.prepend(this->p_reg_node_tree_model->
                  data(node_index,Qt::DisplayRole).toString());
  }

  DlgKeyDetails dlg_key_details(this);
  dlg_key_details.SetValues(nodes,key_name,key_type,key_value);
  dlg_key_details.exec();
  */
}

/*
 * SlotSearchFinished
 */
void MainWindow::SlotSearchFinished() {
  delete this->p_search_thread;
  this->p_search_thread=NULL;
  this->ui->ActionSearch->setEnabled(true);
  // Enable result widget
  this->search_result_widgets.last()->setEnabled(true);
}

/*
 * SlotSearchResultWidgetDoubleClicked
 */
void MainWindow::SlotSearchResultWidgetDoubleClicked(QModelIndex index) {
  SearchResultWidget *p_sender;
  QString path;
  QString match_type;
  QString value;
  QString key="";
  int i;

  if(!index.isValid()) return;

  // Get pointer to sender
  p_sender=(SearchResultWidget*)QObject::sender();

  // Get path and matchtype
  path=p_sender->item(index.row(),0)->text();
  match_type=p_sender->item(index.row(),1)->text();
  value=p_sender->item(index.row(),2)->text();

  if(match_type==tr("Node name")) {
    // Node name is not part of path. Add it
    if(path=="\\") path.append(value);
    else path.append("\\").append(value);
  } else if(match_type==tr("Key name")) {
    // Key name is stored in value
    key=value;
  } else if(match_type==tr("Key value")) {
    // Key name is part of path. Save and remove it
    QStringList nodes=path.split("\\",QString::SkipEmptyParts);
    key=nodes.at(nodes.count()-1);
    // Remove \<key name> from path
    path.chop(key.length()+1);
  }

  // Expand treeview to correct node
  QList<QModelIndex> indexes=
    this->p_reg_node_tree_model->GetIndexListOf(path);
  for(i=0;i<indexes.count();i++) {
    indexes.replace(i,this->p_reg_node_tree_model_proxy->
                        mapFromSource(indexes.at(i)));
    this->p_node_tree->expand(indexes.at(i));
  }
  if(indexes.count()>0) {
    // Scroll to last expanded node, select it and update widgets
    this->p_node_tree->scrollTo(indexes.at(indexes.count()-1),
                                QAbstractItemView::PositionAtCenter);
    this->p_node_tree->selectionModel()->clear();
    this->p_node_tree->selectionModel()->
      select(indexes.at(indexes.count()-1),
             QItemSelectionModel::ClearAndSelect |
               QItemSelectionModel::Rows |
               QItemSelectionModel::Current);
    // TODO: This does not work!!
    this->SlotNodeTreeClicked(indexes.at(indexes.count()-1));
  }

  // Select correct key if search matched on keay name / value
  if(key!="") {
    int row=this->p_reg_key_table_model->GetKeyRow(key);
    this->p_key_table->clearSelection();
    this->p_key_table->scrollTo(this->p_reg_key_table_model->index(row,0),
                                QAbstractItemView::PositionAtCenter);
    this->p_key_table->selectRow(row);
    this->SlotKeyTableClicked(this->p_reg_key_table_model->index(row,0));
  }
}

/*
 * SlotTabCloseButtonClicked
 */
void MainWindow::SlotTabCloseButtonClicked(int index) {
  // Delete tab widget and remove tab
  this->p_tab_widget->removeTab(index);
  delete this->search_result_widgets.at(index-1);
  this->search_result_widgets.removeAt(index-1);
}

/*
 * SlotRecentlyOpenedFileClicked
 */
void MainWindow::SlotRecentlyOpenedFileClicked(bool checked) {
  Q_UNUSED(checked)

  QAction *p_sender=(QAction*)QObject::sender();
  this->OpenHive(p_sender->text());
}

/*
 * SlotAddNode
 */
void MainWindow::SlotAddNode(QModelIndex index) {
  QString node_path;
  int new_node_id;

  if(!index.isValid()) return;

  // Map proxy index to tree model index and get node path
  index=this->p_reg_node_tree_model_proxy->mapToSource(index);
  node_path=this->p_reg_node_tree_model->GetNodePath(index);

  // Query user for a node name
  bool ok=false;
  QString node_name=QInputDialog::getText(this,
                                          tr("Add node"),
                                          tr("Please specify a name for the new node"),
                                          QLineEdit::Normal,
                                          QString(),
                                          &ok);
  if(ok) {
    if((new_node_id=this->p_hive->AddNode(node_path,node_name))==0) {
      // TODO: Get error message and display it
      QMessageBox::critical(this,
                            tr("Error"),
                            tr("Unable to create node '%1\\%2'!")
                              .arg(node_path,node_name));
    } else {
      // Add node to model. We have to pass node_name as Ascii as utf8 names are
      // not supported inside hives!
      QModelIndex new_node_index=
        this->p_reg_node_tree_model->AddNode(this->p_hive,
                                             index,
                                             new_node_id,
                                             node_name.toAscii());
      // Now that node has been added, expand parent and select new node
      this->p_node_tree->expand(
        this->p_reg_node_tree_model_proxy->mapFromSource(index));
      new_node_index=
        this->p_reg_node_tree_model_proxy->mapFromSource(new_node_index);
      this->p_node_tree->scrollTo(new_node_index,
                                  QAbstractItemView::PositionAtCenter);
      this->p_node_tree->selectionModel()->clear();
      this->p_node_tree->selectionModel()->
        select(new_node_index,
               QItemSelectionModel::ClearAndSelect |
                 QItemSelectionModel::Rows |
                 QItemSelectionModel::Current);
      // And finally update key table
      this->SlotNodeTreeClicked(new_node_index);
    }
  }
}

/*
 * SlotDeleteNode
 */
void MainWindow::SlotDeleteNode(QModelIndex index) {
  QString node_path;

  if(!index.isValid()) return;

  // Map proxy index to tree model index and get node path
  index=this->p_reg_node_tree_model_proxy->mapToSource(index);
  node_path=this->p_reg_node_tree_model->GetNodePath(index);

  if(QMessageBox::warning(this,
                          tr("Delete node"),
                          tr("Are you sure you want to remove the node '%1' and all of its child nodes?").arg(node_path),
                          QMessageBox::Yes,
                          QMessageBox::No)==QMessageBox::Yes)
  {
    // Remove node from hive
    if(!this->p_hive->DeleteNode(node_path)) {
      // TODO: Get error message and display it
      QMessageBox::critical(this,
                            tr("Error"),
                            tr("Unable to delete node '%1'!")
                              .arg(node_path));
      return;
    }

    // Remove node from tree model and select nearest node
    QModelIndex next_node_index=this->p_reg_node_tree_model->RemoveNode(index);
    next_node_index=
      this->p_reg_node_tree_model_proxy->mapFromSource(next_node_index);
    this->p_node_tree->selectionModel()->clear();
    this->p_node_tree->selectionModel()->
      select(next_node_index,
             QItemSelectionModel::ClearAndSelect |
               QItemSelectionModel::Rows |
               QItemSelectionModel::Current);
    // And finally update key table
    this->SlotNodeTreeClicked(next_node_index);
  }
}

/*******************************************************************************
 * Private
 ******************************************************************************/

/*
 * OpenHive
 */
void MainWindow::OpenHive(QString hive_file) {
  // Update last open location
  this->p_settings->SetLastOpenLocation(
    hive_file.left(hive_file.lastIndexOf(QDir::separator())));

  // If another hive is currently open, close it
  if(this->is_hive_open) this->on_action_Close_hive_triggered();

  // Try to open hive
  if(!this->p_hive->Open(hive_file,!this->is_hive_writable)) {
    QMessageBox::critical(this,
                          tr("Error opening hive file"),
                          tr("Unable to open file '%1'").arg(hive_file));
    return;
  }

  // Create tree model & proxy
  this->p_reg_node_tree_model=new RegistryNodeTreeModel(this->p_hive);
  this->p_reg_node_tree_model_proxy=new RegistryNodeTreeModelProxy(this);
  //this->p_reg_node_tree_model_proxy->setDynamicSortFilter(true);
  this->p_reg_node_tree_model_proxy->
    setSourceModel(this->p_reg_node_tree_model);
  this->p_node_tree->setModel(this->p_reg_node_tree_model_proxy,
                              this->is_hive_writable);

  this->is_hive_open=true;

  // Enable data interpreter
  this->p_hex_edit_widget->setEnabled(true);

  // Update window title
  this->UpdateWindowTitle(hive_file);

  // Update menu states
  this->UpdateMenuStates();

  // Add file to recent list and update recently opened menu
  this->p_settings->AddRecentFile(hive_file);
  this->UpdateRecentlyOpenedMenu();
}

/*
 * UpdateWindowTitle
 */
void MainWindow::UpdateWindowTitle(QString filename) {
  if(filename=="") {
    this->setWindowTitle(QString("%1 v%2").arg(APP_TITLE,APP_VERSION));
  } else {
    this->setWindowTitle(QString("%1 v%2 - %3").arg(APP_TITLE,
                                                    APP_VERSION,
                                                    filename.toLocal8Bit()
                                                      .constData()));
    if(!this->is_hive_writable) {
      this->setWindowTitle(this->windowTitle().append(QString(" (%1)")
                                                        .arg(tr("read-only"))));
    }
  }
}

/*
 * UpdateMenuStates
 */
void MainWindow::UpdateMenuStates() {
  if(this->is_hive_open) {
    this->ui->action_Close_hive->setEnabled(true);
    this->ui->ActionEnableWriteSupport->setEnabled(true);
    this->ui->ActionSearch->setEnabled(true);
    this->ui->ActionEnableWriteSupport->setEnabled(true);
    this->ui->ActionGenerateReport->setEnabled(true);
    this->ui->ActionReloadReportTemplates->setEnabled(true);
  } else {
    this->ui->action_Close_hive->setEnabled(false);
    this->ui->ActionEnableWriteSupport->setEnabled(false);
    this->ui->ActionSearch->setEnabled(false);
    this->ui->ActionEnableWriteSupport->setEnabled(false);
    this->ui->ActionGenerateReport->setEnabled(false);
    this->ui->ActionReloadReportTemplates->setEnabled(false);
  }
}

/*
 * ClearRecentlyOpenedMenu
 */
void MainWindow::ClearRecentlyOpenedMenu() {
  QAction *p_action;

  // Remove existing menu entries
  QList<QAction*> menu_entries=this->p_recently_opened_menu->actions();
  QListIterator<QAction*> it_me(menu_entries);
  while(it_me.hasNext()) {
    p_action=it_me.next();
    this->p_recently_opened_menu->removeAction(p_action);
    delete p_action;
  }
}

/*
 * UpdateRecentlyOpenedMenu
 */
void MainWindow::UpdateRecentlyOpenedMenu() {
  QStringList recent_files=this->p_settings->GetRecentFiles();
  QAction *p_action;

  // Remove existing menu entries
  this->ClearRecentlyOpenedMenu();

  // If there are no recent files, disable submenu and return
  if(recent_files.isEmpty()) {
    this->ui->ActionRecentlyOpened->setEnabled(false);
    return;
  } else {
    this->ui->ActionRecentlyOpened->setEnabled(true);
  }

  // Add recently opened files to menu
  QListIterator<QString> it_rf(recent_files);
  while(it_rf.hasNext()) {
    // Create menu entry
    p_action=new QAction(it_rf.next(),this->p_recently_opened_menu);
    // Connect it to us
    this->connect(p_action,
                  SIGNAL(triggered(bool)),
                  this,
                  SLOT(SlotRecentlyOpenedFileClicked(bool)));
    // Add it to submenu
    this->p_recently_opened_menu->addAction(p_action);
  }
}

/*
 * UpdateEnableWriteSupportMenu
 */
void MainWindow::UpdateEnableWriteSupportMenu() {
  if(!this->is_hive_writable) {
    this->ui->ActionEnableWriteSupport->setText(tr("Enable &write support"));
    this->p_node_tree->SetWritable(false);
    this->p_key_table->SetWritable(false);
  } else {
    this->ui->ActionEnableWriteSupport->setText(tr("Disable &write support"));
    this->p_node_tree->SetWritable(true);
    this->p_key_table->SetWritable(true);
  }
}

/*
 * SaveHiveChanges
 */
bool MainWindow::SaveHiveChanges() {
  if(!this->is_hive_open) return true;
  if(!this->is_hive_writable) return true;
  if(!this->p_hive->HasChangesToCommit()) return true;

  // There are unsaved changes, ask user if we should commit them
  if(QMessageBox::warning(this,
                          tr("Save changes"),
                          tr("If you don't save your changes now, they will be lost. Do you want to save them?"),
                          QMessageBox::Yes,
                          QMessageBox::No)==QMessageBox::Yes)
  {
    if(!this->p_hive->CommitChanges()) {
      // TODO: Get error message
      QMessageBox::critical(this,
                            tr("Saving changes"),
                            tr("Unable to save changes!"));
      return false;
    }
  }
  return true;
}
