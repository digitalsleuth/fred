#ifndef REGISTRYNODETREE_H
#define REGISTRYNODETREE_H

#include <QTreeView>
#include <QAbstractItemModel>

class RegistryNodeTree : public QTreeView {
  Q_OBJECT

  public:
    RegistryNodeTree(QWidget *p_parent=0);

    void setModel(QAbstractItemModel *p_model);

  //protected:
  //  int sizeHintForColumn(int column) const;
};

#endif // REGISTRYNODETREE_H
