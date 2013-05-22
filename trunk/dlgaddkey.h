#ifndef DLGADDKEY_H
#define DLGADDKEY_H

#include <QDialog>

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

  private:
    Ui::DlgAddKey *ui;
};

#endif // DLGADDKEY_H
