#ifndef REGISTRYKEYTABLE_H
#define REGISTRYKEYTABLE_H

#include <QTableView>

class RegistryKeyTable : public QTableView {
  Q_OBJECT

  public:
    RegistryKeyTable(QWidget *p_parent=0);

    void setModel(QAbstractItemModel *p_model);

  protected:
    int sizeHintForColumn(int column) const;
};

#endif // REGISTRYKEYTABLE_H
