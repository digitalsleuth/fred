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

#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QDesktopWidget>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlgabout.h"
#include "dlgkeydetails.h"

#include "compileinfo.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Initialize private vars
  this->hhive=NULL;
  this->is_hive_open=false;
  this->p_reg_node_tree_model=NULL;
  this->p_reg_key_table_model=NULL;

  // Set window title
  this->UpdateWindowTitle();

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

  // Set last open location to home dir
  this->last_open_location=QDir::homePath();
}

MainWindow::~MainWindow() {
  if(this->is_hive_open) {
    hivex_close(this->hhive);
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

  // Update last open location
  this->last_open_location=hive_file.left(hive_file.
                                            lastIndexOf(QDir::separator()));

  // If another hive is currently open, close it
  if(this->is_hive_open) this->on_action_Close_hive_triggered();

  // Try to open hive
  this->hhive=hivex_open(hive_file.toAscii().constData(),0);
  if(this->hhive==NULL) {
    QMessageBox::critical(this,
                          tr("Error opening hive file"),
                          tr("Unable to open file '%1'").arg(hive_file));
    return;
  }

  // Create tree model
  hive_node_h root_node=hivex_root(hhive);
  if(root_node==0) {
    QMessageBox::critical(this,
                          tr("Error opening hive file"),
                          tr("This hive seems to have no root node!")
                            .arg(hive_file));
    return;
  }
  this->p_reg_node_tree_model=
    new RegistryNodeTreeModel(this->hhive,
                              root_node,
                              this->ui->RegNodeTree);
  this->ui->RegNodeTree->setModel(this->p_reg_node_tree_model);

  this->is_hive_open=true;
  this->ui->action_Close_hive->setEnabled(true);
  this->UpdateWindowTitle(hive_file);
}

void MainWindow::on_action_Close_hive_triggered() {
  if(this->is_hive_open) {
    // Delete models
    if(this->p_reg_node_tree_model!=NULL) {
      delete this->p_reg_node_tree_model;
      this->p_reg_node_tree_model=NULL;
    }
    if(this->p_reg_key_table_model!=NULL) {
      delete this->p_reg_key_table_model;
      this->p_reg_key_table_model=NULL;
    }

    // Close hive
    hivex_close(this->hhive);

    this->is_hive_open=false;
    this->ui->action_Close_hive->setEnabled(false);
    this->UpdateWindowTitle();
  }
}

void MainWindow::on_actionAbout_Qt_triggered() {
  QMessageBox::aboutQt(this,tr("About Qt"));
}

void MainWindow::on_RegNodeTree_clicked(QModelIndex index) {
  QStringList nodes;

  //Built node path
  nodes.clear();
  nodes.append(this->p_reg_node_tree_model->
                 data(index,Qt::DisplayRole).toString());
  while(this->p_reg_node_tree_model->parent(index)!=QModelIndex()) {
    // Prepend all parent nodes
    index=this->p_reg_node_tree_model->parent(index);
    nodes.prepend(this->p_reg_node_tree_model->
                  data(index,Qt::DisplayRole).toString());
  }

  // Get hive_node handle for current node
  hive_node_h hive_node=hivex_root(this->hhive);
  QString cur_node;
  for(QStringList::iterator it=nodes.begin();it!=nodes.end();++it) {
    cur_node=*it;
    hive_node=hivex_node_get_child(this->hhive,
                                   hive_node,
                                   cur_node.toAscii().constData());
  }

  // Create table model and attach it to the table view
  if(this->p_reg_key_table_model!=NULL) delete this->p_reg_key_table_model;
  this->p_reg_key_table_model=new RegistryKeyTableModel(this->hhive,
                                                        hive_node,
                                                        this->ui->RegKeyTable);
  this->ui->RegKeyTable->setModel(this->p_reg_key_table_model);

  // Resize table rows / columns to fit data
  this->ui->RegKeyTable->resizeColumnsToContents();
  this->ui->RegKeyTable->horizontalHeader()->stretchLastSection();
}

void MainWindow::UpdateWindowTitle(QString filename) {
  if(filename=="") {
    this->setWindowTitle(QString().sprintf("%s v%s",APP_TITLE,APP_VERSION));
  } else {
    this->setWindowTitle(QString().sprintf("%s v%s - %s",
                                           APP_TITLE,
                                           APP_VERSION,
                                           filename.toLocal8Bit().constData()));
  }
}

void MainWindow::on_actionAbout_fred_triggered() {
  DlgAbout dlg_about(this);
  dlg_about.exec();
}

void MainWindow::on_RegNodeTree_activated(QModelIndex index) {
  // Also update when pressing ENTER
  this->on_RegNodeTree_clicked(index);
}

void MainWindow::on_RegKeyTable_doubleClicked(QModelIndex index) {
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
    .toString();
  key_index=this->p_reg_key_table_model->index(index.row(),2);
  key_value=this->p_reg_key_table_model->data(key_index,
                                              RegistryKeyTableModel::
                                                AdditionalRoles_GetRawData)
    .toByteArray();

  // Get current node
  node_index=this->ui->RegNodeTree->currentIndex();

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
}
