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

  this->p_node_tree=new QTreeView(this->p_horizontal_splitter);
  this->p_node_tree->setHeaderHidden(true);

  this->p_vertical_splitter=new QSplitter(this->p_horizontal_splitter);
  this->p_vertical_splitter->setOrientation(Qt::Vertical);

  this->p_key_table=new QTableView(this->p_vertical_splitter);
  this->p_key_table->setSelectionBehavior(QAbstractItemView::SelectRows);

  this->p_horizontal_splitter2=new QSplitter(this->p_vertical_splitter);
  this->p_horizontal_splitter2->setOrientation(Qt::Horizontal);

  this->p_hex_edit_widget=new QWidget(this->p_horizontal_splitter2);
  this->p_hex_edit_layout=new QVBoxLayout(this->p_hex_edit_widget);
  this->p_hex_edit_layout->setContentsMargins(0,0,0,0);
  this->p_hex_edit=new QHexEdit();
  this->p_hex_edit->setReadOnly(true);
  this->p_hex_edit_status_bar=new QLabel();

  this->p_data_interpreter=new DataInterpreter(this->p_horizontal_splitter2);

  // Make sure hex viewer font is monospaced.
  QFont mono_font("Monospace");
  mono_font.setStyleHint(QFont::TypeWriter);
  this->p_hex_edit->setFont(mono_font);

  // Lay out widgets
  this->p_hex_edit_layout->addWidget(this->p_hex_edit);
  this->p_hex_edit_layout->addWidget(this->p_hex_edit_status_bar);

  this->p_horizontal_splitter2->addWidget(this->p_hex_edit_widget);
  this->p_horizontal_splitter2->addWidget(this->p_data_interpreter);

  this->p_vertical_splitter->addWidget(this->p_key_table);
  this->p_vertical_splitter->addWidget(this->p_horizontal_splitter2);

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

  QSizePolicy hex_edit_widget_policy=this->p_hex_edit_widget->sizePolicy();
  hex_edit_widget_policy.setVerticalStretch(2);
  hex_edit_widget_policy.setHorizontalStretch(200);
  this->p_hex_edit_widget->setSizePolicy(hex_edit_widget_policy);

  QSizePolicy data_interpreter_policy=this->p_data_interpreter->sizePolicy();
  data_interpreter_policy.setVerticalStretch(2);
  data_interpreter_policy.setHorizontalStretch(0);
  this->p_data_interpreter->setSizePolicy(data_interpreter_policy);

  // Connect signals
  this->connect(this->p_node_tree,
                SIGNAL(clicked(QModelIndex)),
                this,
                SLOT(SlotNodeTreeClicked(QModelIndex)));
  this->connect(this->p_node_tree,
                SIGNAL(activated(QModelIndex)),
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
  this->connect(this->p_hex_edit,
                SIGNAL(currentAddressChanged(int)),
                this,
                SLOT(SlotHexEditAddressChanged(int)));

  // Add central widget
  this->setContentsMargins(4,4,4,0);
  this->setCentralWidget(this->p_horizontal_splitter);

  // Set window title
  this->UpdateWindowTitle();

  // Set last open location to home dir
  this->last_open_location=QDir::homePath();

  // Load report templates and update menu
  this->p_data_reporter=new DataReporter();
  this->p_data_reporter->LoadReportTemplates();
  this->UpdateDataReporterMenu();
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
                              root_node);
  this->p_node_tree->setModel(this->p_reg_node_tree_model);

  this->is_hive_open=true;
  this->ui->action_Close_hive->setEnabled(true);
  this->ui->MenuReports->setEnabled(true);
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

    // Remove any data from hex edit and data interpreter
    this->p_hex_edit->setData(QByteArray());
    this->p_hex_edit_status_bar->setText("");
    this->p_data_interpreter->ClearValues();

    // Close hive
    hivex_close(this->hhive);

    this->is_hive_open=false;
    this->ui->action_Close_hive->setEnabled(false);
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

void MainWindow::SlotNodeTreeClicked(QModelIndex index) {
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
                                                        hive_node);
  this->p_key_table->setModel(this->p_reg_key_table_model);

  // Resize table rows / columns to fit data
  this->p_key_table->resizeColumnsToContents();
  this->p_key_table->horizontalHeader()->stretchLastSection();
}

void MainWindow::SlotKeyTableDoubleClicked(QModelIndex index) {
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
}

void MainWindow::SlotKeyTableClicked(QModelIndex index) {
  if(!index.isValid()) return;

  this->selected_key_value=
    this->p_reg_key_table_model->data(this->p_reg_key_table_model->
                                        index(index.row(),2),
                                      RegistryKeyTableModel::
                                        AdditionalRoles_GetRawData)
                                          .toByteArray();
  this->p_hex_edit->setData(this->selected_key_value);
}

void MainWindow::SlotHexEditAddressChanged(int hex_offset) {
  // Update hex edit status bar
  this->p_hex_edit_status_bar->
    setText(QString().sprintf("Byte offset: 0x%04X (%u)",hex_offset,hex_offset));
  // Update data interpreter
  this->UpdateDataInterpreter(hex_offset);
}

void MainWindow::SlotReportClicked() {
  // Get report category and name from sender and it's parent
  QString category=((QMenu*)((QAction*)QObject::sender())->parent())->title();
  QString report=((QAction*)QObject::sender())->text();

  QString report_content=this->p_data_reporter->GenerateReport(this->hhive,
                                                               category,
                                                               report);

  QMessageBox::information(this,report,report_content);

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

void MainWindow::UpdateDataInterpreter(int hex_offset) {
  QDateTime date_time;
  const char *p_data;
  int remaining_data_len;

  // Remove all old values from data interpreter
  this->p_data_interpreter->ClearValues();

  // Calculate how many bytes are remainig after current offset
  remaining_data_len=this->selected_key_value.size()-hex_offset;
  if(!remaining_data_len>0) {
    // Nothing to show
    return;
  }

  // Get pointer to data at current offset
  p_data=this->selected_key_value.constData();
  p_data+=hex_offset;

  #define rotl32(x,n)   (((x) << n) | ((x) >> (32 - n)))
  //#define rotr32(x,n)   (((x) >> n) | ((x) << (32 - n)))
  #define rotl64(x,n)   (((x) << n) | ((x) >> (64 - n)))
  //#define rotr64(x,n)   (((x) >> n) | ((x) << (64 - n)))

  if(remaining_data_len>=1) {
    this->p_data_interpreter->AddValue("int8:",
                                       QString().sprintf("%d",
                                                         *(int8_t*)p_data));
    this->p_data_interpreter->AddValue("uint8:",
                                       QString().sprintf("%u",
                                                         *(uint8_t*)p_data));
  }
  if(remaining_data_len>=2) {
    this->p_data_interpreter->AddValue("int16:",
                                       QString().sprintf("%d",
                                                         *(int16_t*)p_data));
    this->p_data_interpreter->AddValue("uint16:",
                                       QString().sprintf("%u",
                                                         *(uint16_t*)p_data));
  }
  if(remaining_data_len>=4) {
    this->p_data_interpreter->AddValue("int32:",
                                       QString().sprintf("%d",
                                                         *(int32_t*)p_data));
    this->p_data_interpreter->AddValue("uint32:",
                                       QString().sprintf("%d",
                                                         *(uint32_t*)p_data));
    date_time.setTime_t(*(uint32_t*)p_data);
    this->p_data_interpreter->AddValue("Unixtime:",
                                       date_time.
                                         toString("yyyy/MM/dd hh:mm:ss"));
  }
  if(remaining_data_len>=8) {
    this->p_data_interpreter->AddValue("int64:",
                                       QString().sprintf("%d",
                                                         *(int64_t*)p_data));
    this->p_data_interpreter->AddValue("uint64:",
                                       QString().sprintf("%d",
                                                         *(uint64_t*)p_data));
    date_time.setTime_t((*(uint64_t*)p_data-116444736000000000)/10000000);
    this->p_data_interpreter->AddValue("Win64time:",
                                       date_time.
                                         toString("yyyy/MM/dd hh:mm:ss"));
  }

  #undef rotl32
  #undef rotl64
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

/*
void MainWindow::LoadReportTemplates() {
  //#include <QtXml/QXmlSimpleReader>

  QString report_template="";
  QXmlSimpleReader xml_parser;

  qDebug("Loading report templates...");

  QDir report_dir("../trunk/report_templates/");
  QStringList report_templates=report_dir.entryList(QStringList()<<"*.fred");
  int i=0;

  for(i=0;i<report_templates.count();i++) {
    report_template=report_dir.path();
    report_template.append(QDir::separator());
    report_template.append(report_templates.value(i));

    qDebug("%s",report_template.toAscii().constData());

    QFile *p_report_template_file=new QFile(report_template);
    QXmlInputSource *p_xml_file=new QXmlInputSource(p_report_template_file);
    ReportTemplateXmlHandler *p_report_handler=new ReportTemplateXmlHandler();

    xml_parser.setContentHandler(p_report_handler);
    xml_parser.parse(p_xml_file);

    delete p_report_handler;
    delete p_xml_file;
    delete p_report_template_file;
  }
}
*/
