#ifndef DLGADDKEY_H
#define DLGADDKEY_H

#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QRadioButton>

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
};

#endif // DLGADDKEY_H
