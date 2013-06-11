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

#ifndef DLGADDKEY_H
#define DLGADDKEY_H

#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QRadioButton>

#include <inttypes.h>

namespace Ui {
  class DlgAddKey;
}

class DlgAddKey : public QDialog {
  Q_OBJECT

  public:
    explicit DlgAddKey(QWidget *p_parent=0,
                       QString key_name=QString(),
                       QString key_value_type=QString(),
                       QByteArray key_value=QByteArray());
    ~DlgAddKey();

    QString KeyName();
    QString KeyType();
    QByteArray KeyValue();

  private slots:
    void on_BtnCancel_clicked();
    void on_BtnOk_clicked();
    void on_CmbKeyType_currentIndexChanged(const QString &arg1);

  private:
    Ui::DlgAddKey *ui;
    QWidget *p_current_widget;
    QWidget *p_line_widget;
    QHBoxLayout *p_line_widget_layout;
    QLineEdit *p_line_widget_line_edit;
    QWidget *p_text_widget;
    QHBoxLayout *p_text_widget_layout;
    QPlainTextEdit *p_text_widget_text_edit;
    QWidget *p_number_widget;
    QHBoxLayout *p_number_widget_layout;
    QLineEdit *p_number_widget_line_edit;
    QRadioButton *p_number_widget_rb_decimal;
    QRadioButton *p_number_widget_rb_hex;
    QWidget *p_binary_widget;
    QHBoxLayout *p_binary_widget_layout;

    void CreateValueWidgets();
    void DestroyValueWidgets();
    void SetValueWidgetData(QByteArray &key_value, QString &key_value_type);
    QByteArray GetValueWidgetData();
    int ToUtf16LeBuf(uint16_t **pp_buf,const uint16_t *p_data, int ascii_len);
};

#endif // DLGADDKEY_H
