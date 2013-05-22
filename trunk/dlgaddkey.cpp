#include "dlgaddkey.h"
#include "ui_dlgaddkey.h"

#include "registryhive.h"

#include <QString>
#include <QStringList>

DlgAddKey::DlgAddKey(QWidget *p_parent, QString key_name, QString key_value_type, QByteArray key_value) :
  QDialog(p_parent),
  ui(new Ui::DlgAddKey)
{
  ui->setupUi(this);

  // Set dialog title
  if(key_name.isEmpty() && key_value_type.isEmpty() && key_value.isEmpty()) {
    // If no values were passed, we consider this the ddd key dialog
    this->setWindowTitle(tr("Add key"));
  } else {
    // If a value was passed, this is considered the edit key dialog
    this->setWindowTitle(tr("Edit key"));
    this->ui->EdtKeyName->setEnabled(false);
  }

  // Preload key value type values
  QStringList value_types=RegistryHive::GetKeyValueTypes();
  this->ui->CmbKeyType->addItems(value_types);

  // Load values
  if(!key_name.isEmpty()) this->ui->EdtKeyName->setText(key_name);
  if(!key_value_type.isEmpty())
    this->ui->CmbKeyType->setCurrentIndex(value_types.indexOf(key_value_type));
  if(!key_value.isEmpty()) {
    this->ui->EdtKeyValue->setText(
      RegistryHive::KeyValueToString(key_value,
                                     RegistryHive::StringToKeyValueType(
                                       key_value_type)));
  }
}

DlgAddKey::~DlgAddKey() {
  delete ui;
}

QString DlgAddKey::KeyName() {
  return this->ui->EdtKeyName->text();
}

QString DlgAddKey::KeyType() {
  return this->ui->CmbKeyType->currentText();
}

QByteArray DlgAddKey::KeyValue() {
  return QByteArray("TestTest");
}

void DlgAddKey::on_BtnCancel_clicked() {
  this->reject();
}

void DlgAddKey::on_BtnOk_clicked()
{
  this->accept();
}
