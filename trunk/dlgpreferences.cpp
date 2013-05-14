#include "dlgpreferences.h"
#include "ui_dlgpreferences.h"

#include <QString>
#include <QStringList>
#include <QListIterator>
#include <QModelIndexList>

#include <QDebug>

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

void DlgPreferences::LoadPreferences() {

  // Populate report location list
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

void DlgPreferences::on_BtnAddReportLoc_clicked() {
  // TODO
}

void DlgPreferences::on_BtnEditReportLoc_clicked() {
  // TODO
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
  // TODO: takeItem/insertItem
}

void DlgPreferences::on_BtnMoveReportLocDown_clicked() {
  // TODO: takeItem/insertItem
}


void DlgPreferences::on_BtnReset_clicked() {
  // TODO
}
