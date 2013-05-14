#include "dlgpreferences.h"
#include "ui_dlgpreferences.h"

DlgPreferences::DlgPreferences(Settings *p_sets, QWidget *p_parent) :
  QDialog(p_parent), ui(new Ui::DlgPreferences)
{
  ui->setupUi(this);
  this->p_settings=p_sets;
}

DlgPreferences::~DlgPreferences() {
  delete ui;
}

void DlgPreferences::on_BtnCancel_clicked() {
  this->reject();
}
