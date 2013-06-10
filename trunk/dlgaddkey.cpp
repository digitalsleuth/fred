#include "dlgaddkey.h"
#include "ui_dlgaddkey.h"

#include "registryhive.h"

#include <QString>
#include <QStringList>

#include <QDebug>

DlgAddKey::DlgAddKey(QWidget *p_parent,
                     QString key_name,
                     QString key_value_type,
                     QByteArray key_value) :
  QDialog(p_parent),
  ui(new Ui::DlgAddKey)
{
  this->p_current_widget=NULL;
  ui->setupUi(this);

  // Create widgets
  this->CreateValueWidgets();

  // Set dialog title
  if(key_name.isEmpty() && key_value_type.isEmpty() && key_value.isEmpty()) {
    // If no values were passed, we consider this the ddd key dialog
    this->setWindowTitle(tr("Add key"));
  } else {
    // If a value was passed, this is considered the edit key dialog
    this->setWindowTitle(tr("Edit key"));
    this->ui->EdtKeyName->setEnabled(false);
    this->ui->CmbKeyType->setEnabled(false);
  }

  // Preload key value type values
  QStringList value_types=RegistryHive::GetKeyValueTypes();
  this->ui->CmbKeyType->addItems(value_types);

  // Load values
  if(!key_name.isEmpty()) this->ui->EdtKeyName->setText(key_name);
  if(!key_value_type.isEmpty())
    this->ui->CmbKeyType->setCurrentIndex(value_types.indexOf(key_value_type));
  if(!key_value.isEmpty()) this->SetValueWidgetData(key_value,key_value_type);
}

DlgAddKey::~DlgAddKey() {
  this->DestroyValueWidgets();
  delete ui;
}

QString DlgAddKey::KeyName() {
  return this->ui->EdtKeyName->text();
}

QString DlgAddKey::KeyType() {
  return this->ui->CmbKeyType->currentText();
}

QByteArray DlgAddKey::KeyValue() {
  return this->GetValueWidgetData();
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
  if(arg1=="REG_SZ" || arg1=="REG_EXPAND_SZ") {
    // Line edit widget for REG_SZ and REG_EXPAND_SZ
    this->ui->gridLayout->addWidget(this->p_line_widget,2,1);
    this->p_current_widget=this->p_line_widget;
  } else if(arg1=="REG_MULTI_SZ") {
    // Text edit widget for REG_MULTI_SZ
    this->ui->gridLayout->addWidget(this->p_text_widget,2,1);
    this->p_current_widget=this->p_text_widget;
  } else if(arg1=="REG_DWORD" ||
            arg1=="REG_DWORD_BIG_ENDIAN" ||
            arg1=="REG_QWORD")
  {
    // Number widget for REG_DWORD, REG_DWORD_BIG_ENDIAN and REG_QWORD
    this->ui->gridLayout->addWidget(this->p_number_widget,2,1);
    this->p_current_widget=this->p_number_widget;
  } else if(arg1=="REG_BINARY" ||
            arg1=="REG_LINK" ||
            arg1=="REG_RESOURCE_LIST" ||
            arg1=="REG_FULL_RESOURCE_DESC" ||
            arg1=="REG_RESOURCE_REQ_LIST")
  {
    // Binary widget for all other types
    this->ui->gridLayout->addWidget(this->p_binary_widget,2,1);
    this->p_current_widget=this->p_binary_widget;
  }

  if(arg1!="REG_NONE") {
    this->p_current_widget->setVisible(true);
    this->ui->LblKeyValue->setVisible(true);
  } else {
    this->ui->LblKeyValue->setVisible(false);
  }
}

void DlgAddKey::CreateValueWidgets() {
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

void DlgAddKey::DestroyValueWidgets() {
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

void DlgAddKey::SetValueWidgetData(QByteArray &key_value,
                                   QString &key_value_type)
{
  if(key_value_type=="REG_SZ" || key_value_type=="REG_EXPAND_SZ") {
    p_line_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,
                                     RegistryHive::StringToKeyValueType(
                                       key_value_type)));
  } else if(key_value_type=="REG_MULTI_SZ") {
    // TODO: How should text be splitted? Ascii, utfxx ??????
    // p_text_widget_text_edit
  } else if(key_value_type=="REG_DWORD") {
    p_number_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,"int32"));
  } else if(key_value_type=="REG_DWORD_BIG_ENDIAN") {
    p_number_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,"int32",0,0,false));
  } else if(key_value_type=="REG_QWORD") {
    p_number_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,"int64",0,0,false));
  } else if(key_value_type=="REG_BINARY" ||
            key_value_type=="REG_LINK" ||
            key_value_type=="REG_RESOURCE_LIST" ||
            key_value_type=="REG_FULL_RESOURCE_DESC" ||
            key_value_type=="REG_RESOURCE_REQ_LIST")
  {
    // TODO: Set binary data
  }
}

QByteArray DlgAddKey::GetValueWidgetData() {
  QString key_value_type=this->KeyType();

  if(key_value_type=="REG_SZ" || key_value_type=="REG_EXPAND_SZ") {
    // TODO: Won't work! Normally UTF16_LE, but??????
    return QByteArray(p_line_widget_line_edit->text().toLocal8Bit().constData());
  } else if(key_value_type=="REG_MULTI_SZ") {
    // TODO: How should text be concatenated? Ascii, utfxx ??????
    // p_text_widget_text_edit
    return QByteArray();
  } else if(key_value_type=="REG_DWORD") {
    // TODO: When pressing ok, we need to check if conversion to number works!
    // TODO: We need host_to_le32 here!
    int32_t val;
    if(p_number_widget_rb_decimal->isChecked()) {
      val=p_number_widget_line_edit->text().toInt();
    } else {
      val=p_number_widget_line_edit->text().toInt(0,16);
    }
    return QByteArray((char*)&val,4);
  } else if(key_value_type=="REG_DWORD_BIG_ENDIAN") {
    // TODO: Convert to big endian
    return QByteArray();
  } else if(key_value_type=="REG_QWORD") {
    // TODO: We need host_to_le64 here!
    int64_t val;
    if(p_number_widget_rb_decimal->isChecked()) {
      val=p_number_widget_line_edit->text().toLongLong();
    } else {
      val=p_number_widget_line_edit->text().toLongLong(0,16);
    }
    return QByteArray((char*)&val,8);
  } else if(key_value_type=="REG_BINARY" ||
            key_value_type=="REG_LINK" ||
            key_value_type=="REG_RESOURCE_LIST" ||
            key_value_type=="REG_FULL_RESOURCE_DESC" ||
            key_value_type=="REG_RESOURCE_REQ_LIST")
  {
    // TODO: Return binary data
    return QByteArray();
  }
  return QByteArray();
}
