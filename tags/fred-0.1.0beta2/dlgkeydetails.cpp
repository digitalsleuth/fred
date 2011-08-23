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

#include "dlgkeydetails.h"
#include "ui_dlgkeydetails.h"

DlgKeyDetails::DlgKeyDetails(QWidget *p_parent)
  : QDialog(p_parent), ui(new Ui::DlgKeyDetails)
{
  ui->setupUi(this);

  // Init private vars
  this->parent_nodes=QStringList();
  this->key_name=QString();
  this->key_type=QString();
  this->key_value=QByteArray();
  this->p_hex_edit=NULL;
}

DlgKeyDetails::~DlgKeyDetails() {
  if(this->p_hex_edit!=NULL) delete this->p_hex_edit;
  delete ui;
}

void DlgKeyDetails::SetValues(QStringList &r_parent_nodes,
                              QString &r_key_name,
                              QString &r_key_type,
                              QByteArray &r_key_value)
{
  // Save params to local vars
  this->parent_nodes=r_parent_nodes;
  this->key_name=r_key_name;
  this->key_type=r_key_type;
  this->key_value=r_key_value;

  // Set dialog title to complete registry path
  this->setWindowTitle(parent_nodes.join("/").append("/").append(r_key_name));

  // Create read only hex edit widget
  this->p_hex_edit=new QHexEdit();
  this->p_hex_edit->setReadOnly(true);

  // Make sure hex viewer font is monospaced.
  QFont mono_font("Monospace");
  mono_font.setStyleHint(QFont::TypeWriter);
  this->p_hex_edit->setFont(mono_font);

  // Add the hexedit widget to the dialog
  // As we know the dialog uses a box layout...
  ((QBoxLayout*)this->layout())->insertWidget(0,this->p_hex_edit);

  // Add data to hex edit
  this->p_hex_edit->setData(this->key_value);
}

void DlgKeyDetails::changeEvent(QEvent *e) {
  QDialog::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void DlgKeyDetails::on_BtnClose_clicked() {
  this->accept();
}
