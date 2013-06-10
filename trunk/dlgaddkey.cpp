#include "dlgaddkey.h"
#include "ui_dlgaddkey.h"

#include "registryhive.h"

#include <QString>
#include <QStringList>

#include <QDebug>

DlgAddKey::DlgAddKey(QWidget *p_parent, QString key_name, QString key_value_type, QByteArray key_value) :
  QDialog(p_parent),
  ui(new Ui::DlgAddKey)
{
  this->p_current_widget=NULL;
  ui->setupUi(this);

  // Create widgets
  this->CreateWidgets();

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
/*
  if(!key_value.isEmpty()) {
    this->ui->EdtKeyValue->setText(
      RegistryHive::KeyValueToString(key_value,
                                     RegistryHive::StringToKeyValueType(
                                       key_value_type)));
  }
*/
}

DlgAddKey::~DlgAddKey() {
  this->DestroyWidgets();
  delete ui;
}

QString DlgAddKey::KeyName() {
  return this->ui->EdtKeyName->text();
}

QString DlgAddKey::KeyType() {
  return this->ui->CmbKeyType->currentText();
}

QByteArray DlgAddKey::KeyValue() {
  /*
  if(value_type=="REG_NONE") return hive_t_REG_NONE;
  if(value_type=="REG_SZ") return hive_t_REG_SZ;
  if(value_type=="REG_EXPAND_SZ") return hive_t_REG_EXPAND_SZ;
  if(value_type=="REG_BINARY") return hive_t_REG_BINARY;
  if(value_type=="REG_DWORD") return hive_t_REG_DWORD;
  if(value_type=="REG_DWORD_BIG_ENDIAN") return hive_t_REG_DWORD_BIG_ENDIAN;
  if(value_type=="REG_LINK") return hive_t_REG_LINK;
  if(value_type=="REG_MULTI_SZ") return hive_t_REG_MULTI_SZ;
  if(value_type=="REG_RESOURCE_LIST") return hive_t_REG_RESOURCE_LIST;
  if(value_type=="REG_FULL_RESOURCE_DESC")
    return hive_t_REG_FULL_RESOURCE_DESCRIPTOR;
  if(value_type=="REG_RESOURCE_REQ_LIST")
    return hive_t_REG_RESOURCE_REQUIREMENTS_LIST;
  if(value_type=="REG_QWORD") return hive_t_REG_QWORD;

  // I think this might be a good default :-)
  return hive_t_REG_BINARY;
  */



  return QByteArray("TestTest");
}

void DlgAddKey::on_BtnCancel_clicked() {
  this->reject();
}

void DlgAddKey::on_BtnOk_clicked()
{
  this->accept();
}

void DlgAddKey::on_CmbKeyType_currentIndexChanged(const QString &arg1) {
  // Remove current widget from grid layout
  if(this->p_current_widget!=NULL) {
    this->ui->gridLayout->removeWidget(this->p_current_widget);
    this->p_current_widget->setVisible(false);
    this->p_current_widget=NULL;
  }

  // Add new widget for selected value type
  // Line edit widget for REG_SZ and REG_EXPAND_SZ
  if(arg1=="REG_SZ" || arg1=="REG_EXPAND_SZ") {
    this->ui->gridLayout->addWidget(this->p_line_widget,2,1);
    this->p_current_widget=this->p_line_widget;
  }

  // Text edit widget for REG_MULTI_SZ
  if(arg1=="REG_MULTI_SZ") {
    this->ui->gridLayout->addWidget(this->p_text_widget,2,1);
    this->p_current_widget=this->p_text_widget;
  }

  // Number widget for REG_DWORD, REG_DWORD_BIG_ENDIAN and REG_QWORD
  if(arg1=="REG_DWORD" || arg1=="REG_DWORD_BIG_ENDIAN" || arg1=="REG_QWORD") {
    this->ui->gridLayout->addWidget(this->p_number_widget,2,1);
    this->p_current_widget=this->p_number_widget;
  }

  // Binary widget for all other types
  if(arg1=="REG_BINARY" || arg1=="REG_LINK" || arg1=="REG_RESOURCE_LIST" ||
     arg1=="REG_FULL_RESOURCE_DESC" || arg1=="REG_RESOURCE_REQ_LIST")
  {
    this->ui->gridLayout->addWidget(this->p_binary_widget,2,1);
    this->p_current_widget=this->p_binary_widget;
    this->p_current_widget->setVisible(true);
  }

  if(arg1!="REG_NONE") this->p_current_widget->setVisible(true);
  //QSize size=this->sizeHint();
  //this->resize(size);
}

void DlgAddKey::CreateWidgets() {
  this->p_line_widget=new QWidget();
  this->p_line_widget_layout=new QHBoxLayout(this->p_line_widget);
  this->p_line_widget_line_edit=new QLineEdit();
  this->p_line_widget->setContentsMargins(0,0,0,0);
  this->p_line_widget_layout->setContentsMargins(0,0,0,0);
  this->p_line_widget_layout->addWidget(this->p_line_widget_line_edit);

  this->p_text_widget=new QWidget();
  this->p_text_widget_layout=new QHBoxLayout(this->p_text_widget);
  this->p_text_widget_text_edit=new QPlainTextEdit();
  this->p_text_widget->setContentsMargins(0,0,0,0);
  this->p_text_widget_layout->setContentsMargins(0,0,0,0);
  this->p_text_widget_layout->addWidget(this->p_text_widget_text_edit);

  this->p_number_widget=new QWidget();
  this->p_number_widget_layout=new QHBoxLayout(this->p_number_widget);
  this->p_number_widget_line_edit=new QLineEdit();
  this->p_number_widget_rb_decimal=new QRadioButton(tr("Dec base"));
  this->p_number_widget_rb_decimal->setChecked(true);
  this->p_number_widget_rb_hex=new QRadioButton(tr("Hex base"));
  this->p_number_widget->setContentsMargins(0,0,0,0);
  this->p_number_widget_layout->setContentsMargins(0,0,0,0);
  this->p_number_widget_layout->addWidget(this->p_number_widget_line_edit);
  this->p_number_widget_layout->addWidget(this->p_number_widget_rb_decimal);
  this->p_number_widget_layout->addWidget(this->p_number_widget_rb_hex);

  this->p_binary_widget=new QWidget();
  this->p_binary_widget_layout=new QHBoxLayout(this->p_binary_widget);
  this->p_binary_widget->setContentsMargins(0,0,0,0);
  this->p_binary_widget_layout->setContentsMargins(0,0,0,0);
}

void DlgAddKey::DestroyWidgets() {
  delete this->p_line_widget_line_edit;
  delete this->p_line_widget_layout;
  delete this->p_line_widget;

  delete this->p_text_widget_text_edit;
  delete this->p_text_widget_layout;
  delete this->p_text_widget;

  delete this->p_number_widget_rb_hex;
  delete this->p_number_widget_rb_decimal;
  delete this->p_number_widget_line_edit;
  delete this->p_number_widget_layout;
  delete this->p_number_widget;

  delete this->p_binary_widget_layout;
  delete this->p_binary_widget;
}
