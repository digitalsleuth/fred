/*******************************************************************************
* fred Copyright (c) 2011-2013 by Gillen Daniel <gillen.dan@pinguin.lu>        *
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

#ifndef DLGKEYDETAILS_H
#define DLGKEYDETAILS_H

// TODO: Is this dialog still needed in any ways????

#include <QDialog>
#include <QByteArray>

#include "qhexedit/qhexedit.h"

namespace Ui {
  class DlgKeyDetails;
}

class DlgKeyDetails : public QDialog {
  Q_OBJECT

  public:
    explicit DlgKeyDetails(QWidget *p_parent=0);
    ~DlgKeyDetails();

    void SetValues(QStringList &r_parent_nodes,
                   QString &r_key_name,
                   QString &r_key_type,
                   QByteArray &r_key_value);

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void on_BtnClose_clicked();

  private:
    Ui::DlgKeyDetails *ui;
    QStringList parent_nodes;
    QString key_name;
    QString key_type;
    QByteArray key_value;
    QHexEdit *p_hex_edit;
};

#endif // DLGKEYDETAILS_H
