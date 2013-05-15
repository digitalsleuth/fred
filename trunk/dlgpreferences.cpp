#include "dlgpreferences.h"
#include "ui_dlgpreferences.h"

#include <QString>
#include <QStringList>
#include <QListIterator>
#include <QModelIndexList>
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

/*******************************************************************************
 * Public
 ******************************************************************************/

DlgPreferences::DlgPreferences(Settings *p_sets, QWidget *p_parent) :
  QDialog(p_parent), ui(new Ui::DlgPreferences)
{
  ui->setupUi(this);
  this->p_settings=p_sets;

  // Load current values
  this->LoadPreferences();
}

DlgPreferences::~DlgPreferences() {
  delete ui;
}

/*******************************************************************************
 * Private slots
 ******************************************************************************/

void DlgPreferences::on_BtnCancel_clicked() {
  this->reject();
}

void DlgPreferences::on_ListReportLocations_clicked(const QModelIndex &index) {
  if(!index.isValid()) {
    // No valid row selected, disable some buttons
    this->ui->BtnEditReportLoc->setEnabled(false);
    this->ui->BtnRemoveReportLoc->setEnabled(false);
    this->ui->BtnMoveReportLocUp->setEnabled(false);
    this->ui->BtnMoveReportLocDown->setEnabled(false);
    return;
  }

  if(this->ui->ListReportLocations->count()==1) {
    // Only one item left, disable up/down buttons
    this->ui->BtnEditReportLoc->setEnabled(true);
    this->ui->BtnRemoveReportLoc->setEnabled(true);
    this->ui->BtnMoveReportLocUp->setEnabled(false);
    this->ui->BtnMoveReportLocDown->setEnabled(false);
    return;
  }

  if(index.row()==0) {
    // First row selected, disable up button
    this->ui->BtnEditReportLoc->setEnabled(true);
    this->ui->BtnRemoveReportLoc->setEnabled(true);
    this->ui->BtnMoveReportLocUp->setEnabled(false);
    this->ui->BtnMoveReportLocDown->setEnabled(true);
    return;
  }

  if(index.row()==(this->ui->ListReportLocations->count()-1)) {
    // Last row selected, disable down button
    this->ui->BtnEditReportLoc->setEnabled(true);
    this->ui->BtnRemoveReportLoc->setEnabled(true);
    this->ui->BtnMoveReportLocUp->setEnabled(true);
    this->ui->BtnMoveReportLocDown->setEnabled(false);
    return;
  }

  // Any other valid row selected, enable up/down buttons
  this->ui->BtnEditReportLoc->setEnabled(true);
  this->ui->BtnRemoveReportLoc->setEnabled(true);
  this->ui->BtnMoveReportLocUp->setEnabled(true);
  this->ui->BtnMoveReportLocDown->setEnabled(true);
}

void DlgPreferences::on_BtnAddReportLoc_clicked() {
  QString new_loc=QFileDialog::getExistingDirectory(this,
                                                    tr("Select new report "
                                                         "directory"));
  if(!new_loc.isEmpty()) {
    this->ui->ListReportLocations->addItem(new_loc);
  }
}

void DlgPreferences::on_BtnEditReportLoc_clicked() {
  QModelIndex cur_item=this->ui->ListReportLocations->currentIndex();
  if(!cur_item.isValid()) return;

  // Get selected item
  QListWidgetItem *p_item=this->ui->ListReportLocations->item(cur_item.row());

  // Let user select new directory
  QString new_loc=QFileDialog::getExistingDirectory(this,
                                                    tr("Edit report directory"),
                                                    p_item->text());
  if(!new_loc.isEmpty()) {
    p_item->setText(new_loc);
  }
}

void DlgPreferences::on_BtnRemoveReportLoc_clicked() {
  QModelIndex cur_item=this->ui->ListReportLocations->currentIndex();
  if(!cur_item.isValid()) return;

  QListWidgetItem *p_item=
    this->ui->ListReportLocations->takeItem(cur_item.row());
  delete p_item;
  // Update buttons
  this->on_ListReportLocations_clicked(
    this->ui->ListReportLocations->currentIndex());
}

void DlgPreferences::on_BtnMoveReportLocUp_clicked() {
  QModelIndex cur_item=this->ui->ListReportLocations->currentIndex();
  if(!cur_item.isValid() || cur_item.row()==0) return;

  // Move selected item up
  QListWidgetItem *p_item=
    this->ui->ListReportLocations->takeItem(cur_item.row());
  this->ui->ListReportLocations->insertItem(cur_item.row()-1,p_item);

  // Reselect moved item and update buttons
  this->ui->ListReportLocations->setCurrentItem(p_item);
  this->on_ListReportLocations_clicked(
    this->ui->ListReportLocations->currentIndex());
}

void DlgPreferences::on_BtnMoveReportLocDown_clicked() {
  QModelIndex cur_item=this->ui->ListReportLocations->currentIndex();
  if(!cur_item.isValid() ||
     cur_item.row()==(this->ui->ListReportLocations->count()-1))
  {
    return;
  }

  // Move selected item up
  QListWidgetItem *p_item=
    this->ui->ListReportLocations->takeItem(cur_item.row());
  this->ui->ListReportLocations->insertItem(cur_item.row()+1,p_item);

  // Reselect moved item and update buttons
  this->ui->ListReportLocations->setCurrentItem(p_item);
  this->on_ListReportLocations_clicked(
    this->ui->ListReportLocations->currentIndex());
}


void DlgPreferences::on_BtnReset_clicked() {
  if(QMessageBox::warning(this,
                   tr("Reset default settings"),
                   tr("Are you sure to reset all settings to their defaults?"),
                   QMessageBox::No,
                   QMessageBox::Yes)==QMessageBox::Yes)
  {
    this->p_settings->Reset();
  }
}

void DlgPreferences::on_BtnOk_clicked() {
  this->SavePreferences();
  this->accept();
}

/*******************************************************************************
 * Private
 ******************************************************************************/

void DlgPreferences::LoadPreferences() {

  // Populate report location list
  this->ui->ListReportLocations->clear();
  QStringList report_dirs=this->p_settings->GetReportTemplateDirs();
  QListIterator<QString> it_report_dirs(report_dirs);
  if(!report_dirs.isEmpty()) {
    while(it_report_dirs.hasNext()) {
      this->ui->ListReportLocations->addItem(it_report_dirs.next());
    }
    this->ui->ListReportLocations->setCurrentRow(0);
    this->ui->BtnEditReportLoc->setEnabled(true);
    this->ui->BtnRemoveReportLoc->setEnabled(true);
    this->ui->BtnMoveReportLocDown->setEnabled(true);
  }
}

void DlgPreferences::SavePreferences() {

  // Save report location list
  QStringList report_dirs;
  for(int i=0;i<this->ui->ListReportLocations->count();i++) {
    report_dirs.append(this->ui->ListReportLocations->item(i)->text());
  }
  this->p_settings->SetReportTemplateDirs(report_dirs);
}
