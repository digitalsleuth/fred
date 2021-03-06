/*******************************************************************************
* fred Copyright (c) 2011-2012 by Gillen Daniel <gillen.dan@pinguin.lu>        *
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlgabout.h"
#include "dlgkeydetails.h"
#include "dlgreportviewer.h"
#include "dlgsearch.h"

#include "compileinfo.h"

MainWindow::MainWindow(ArgParser *p_arg_parser) :
  QMainWindow(0), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Initialize private vars
  this->p_args=p_arg_parser;
  this->p_hive=new RegistryHive(this);
  this->is_hive_open=false;
  this->p_reg_node_tree_model=NULL;
  this->p_reg_key_table_model=NULL;
  this->p_search_thread=NULL;
  this->search_result_widgets.clear();

  // Check for ~/.fred config dir
  this->CheckUserConfigDir();

  // Set main window size
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

  // Create widgets
  this->p_horizontal_splitter=new QSplitter();
  this->p_horizontal_splitter->setOrientation(Qt::Horizontal);

  this->p_node_tree=new RegistryNodeTree(this->p_horizontal_splitter);
  this->p_node_tree->setHeaderHidden(true);

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

  // Add central widget
  this->setCentralWidget(this->p_horizontal_splitter);
  this->centralWidget()->setContentsMargins(4,4,4,0);

  // Set window title
  this->UpdateWindowTitle();

  // Set last open location to home dir
  this->last_open_location=QDir::homePath();

  // Load report templates and update menu
  this->p_data_reporter=new DataReporter();
  // Load reports from system wide include dir
  this->p_data_reporter->LoadReportTemplates(FRED_REPORT_TEMPLATE_DIR);
  // Load user's report templates
  this->p_data_reporter->LoadReportTemplates(QDir::homePath()
                                               .append(QDir::separator())
                                               .append(".fred")
                                               .append(QDir::separator())
                                               .append("report_templates"));
  this->UpdateDataReporterMenu();

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

MainWindow::~MainWindow() {
  if(this->is_hive_open) {
    this->p_hive->Close();
  }
  delete ui;
}

void MainWindow::on_action_Quit_triggered() {
  qApp->exit();
}

void MainWindow::on_action_Open_hive_triggered() {
  QString hive_file="";

  hive_file=QFileDialog::getOpenFileName(this,
                                    tr("Open registry hive"),
                                    this->last_open_location,
                                    tr("All files (*)"));
  if(hive_file=="") return;

  this->OpenHive(hive_file);
}

void MainWindow::on_action_Close_hive_triggered() {
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
      delete this->p_reg_node_tree_model;
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
    this->ui->action_Close_hive->setEnabled(false);
    this->ui->ActionSearch->setEnabled(false);
    this->ui->MenuReports->setEnabled(false);
    this->UpdateWindowTitle();
  }
}

void MainWindow::on_actionAbout_Qt_triggered() {
  QMessageBox::aboutQt(this,tr("About Qt"));
}

void MainWindow::on_actionAbout_fred_triggered() {
  DlgAbout dlg_about(this);
  dlg_about.exec();
}

void MainWindow::on_ActionSearch_triggered() {
  DlgSearch dlg_search(this);
  if(dlg_search.exec()==QDialog::Accepted) {
    // Create search thread and connect needed signals/slots
    this->p_search_thread=new ThreadSearch(this);
    QList<QByteArray> keywords;
    keywords.append(QByteArray(QString("Windows").toAscii()));

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

void MainWindow::SlotNodeTreeClicked(QModelIndex index) {
  QString node_path;

  if(!index.isValid()) return;

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
  this->p_key_table->setModel(this->p_reg_key_table_model);
  // Set focus back to nodetree to be able to navigate with keyboard
  this->p_node_tree->setFocus();
}

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

void MainWindow::SlotReportClicked() {
  // Get report category and name from sender and it's parent
  QString category=((QMenu*)((QAction*)QObject::sender())->parent())->title();
  QString report=((QAction*)QObject::sender())->text();

  // Generate report
  QString report_content=this->p_data_reporter->GenerateReport(this->p_hive,
                                                               category,
                                                               report);

  // Show result in report viewer
  if(report_content!=QString()) {
    DlgReportViewer *p_dlg_report_view=new DlgReportViewer(report_content,this);
    p_dlg_report_view->exec();
    delete p_dlg_report_view;
  } else {
    // TODO: Something went wrong!
  }
}

void MainWindow::SlotSearchFinished() {
  delete this->p_search_thread;
  this->p_search_thread=NULL;
  this->ui->ActionSearch->setEnabled(true);
  // Enable result widget
  this->search_result_widgets.last()->setEnabled(true);
}

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
    this->p_node_tree->expand(indexes.at(i));
  }
  if(indexes.count()>0) {
    // Scroll to last expanded node, select it and update widgets
    this->p_node_tree->scrollTo(indexes.at(indexes.count()-1),
                                QAbstractItemView::PositionAtCenter);
    this->p_node_tree->selectionModel()->clear();
    this->p_node_tree->selectionModel()->
      select(indexes.at(indexes.count()-1),
             QItemSelectionModel::Select);
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

void MainWindow::SlotTabCloseButtonClicked(int index) {
  // Delete tab widget and remove tab
  this->p_tab_widget->removeTab(index);
  delete this->search_result_widgets.at(index-1);
  this->search_result_widgets.removeAt(index-1);
}

void MainWindow::CheckUserConfigDir() {
  QString user_config_dir=QDir::homePath()
                            .append(QDir::separator())
                            .append(".fred");
  if(!QDir(user_config_dir).exists()) {
    // User config dir does not exists, try to create it
    if(!QDir().mkpath(user_config_dir)) {
      // TODO: Maybe warn user
      return;
    }
    user_config_dir.append(QDir::separator()).append("report_templates");
    if(!QDir().mkpath(user_config_dir)) {
      // TODO: Maybe warn user
      return;
    }
  }
}

void MainWindow::UpdateWindowTitle(QString filename) {
  if(filename=="") {
    this->setWindowTitle(QString("%1 v%2").arg(APP_TITLE,APP_VERSION));
  } else {
    this->setWindowTitle(QString("%1 v%2 - %3").arg(APP_TITLE,
                                                    APP_VERSION,
                                                    filename.toLocal8Bit()
                                                      .constData()));
  }
}

void MainWindow::UpdateDataReporterMenu() {
  int i=0,ii=0;
  QMenu *p_category_entry;
  QAction *p_report_entry;
  QStringList categories=this->p_data_reporter->GetAvailableReportCategories();
  QStringList reports;

  for(i=0;i<categories.count();i++) {
    // First create category submenu
    p_category_entry=this->ui->MenuReports->addMenu(categories.value(i));
    // Now add category reports
    reports=this->p_data_reporter->GetAvailableReports(categories.value(i));
    for(ii=0;ii<reports.count();ii++) {
      p_report_entry=new QAction(reports.value(ii),p_category_entry);
      p_category_entry->addAction(p_report_entry);
      this->connect(p_report_entry,
                    SIGNAL(triggered()),
                    this,
                    SLOT(SlotReportClicked()));
    }
  }
}

void MainWindow::OpenHive(QString hive_file) {
  // Update last open location
  this->last_open_location=hive_file.left(hive_file.
                                            lastIndexOf(QDir::separator()));

  // If another hive is currently open, close it
  if(this->is_hive_open) this->on_action_Close_hive_triggered();

  // Try to open hive
  if(!this->p_hive->Open(hive_file)) {
    QMessageBox::critical(this,
                          tr("Error opening hive file"),
                          tr("Unable to open file '%1'").arg(hive_file));
    return;
  }

  // Create tree model
  this->p_reg_node_tree_model=
    new RegistryNodeTreeModel(this->p_hive);
  this->p_node_tree->setModel(this->p_reg_node_tree_model);

  this->is_hive_open=true;
  this->ui->action_Close_hive->setEnabled(true);
  this->ui->ActionSearch->setEnabled(true);
  this->ui->MenuReports->setEnabled(true);

  // Enable data interpreter
  this->p_hex_edit_widget->setEnabled(true);

  this->UpdateWindowTitle(hive_file);
}
