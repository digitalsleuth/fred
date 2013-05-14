#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>

#include "settings.h"

namespace Ui {
  class DlgPreferences;
}

class DlgPreferences : public QDialog {
  Q_OBJECT

  public:
    explicit DlgPreferences(Settings *p_sets, QWidget *p_parent=0);
    ~DlgPreferences();

  private slots:
    void on_BtnCancel_clicked();

  private:
    Ui::DlgPreferences *ui;
    Settings *p_settings;
};

#endif // DLGPREFERENCES_H
