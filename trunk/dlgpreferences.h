#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <QModelIndex>

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
    void on_ListReportLocations_clicked(const QModelIndex &index);
    void on_BtnAddReportLoc_clicked();
    void on_BtnEditReportLoc_clicked();
    void on_BtnRemoveReportLoc_clicked();
    void on_BtnMoveReportLocUp_clicked();
    void on_BtnMoveReportLocDown_clicked();
    void on_BtnReset_clicked();

    void on_BtnOk_clicked();

  private:
    Ui::DlgPreferences *ui;
    Settings *p_settings;

    void LoadPreferences();
    void SavePreferences();
};

#endif // DLGPREFERENCES_H
