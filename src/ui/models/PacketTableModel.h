#pragma once

#include "decode/PacketInfo.h"

#include <QAbstractTableModel>

#include <vector>

namespace sniffles::ui {
class PacketTableModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit PacketTableModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  void AddPacket(const decode::PacketInfo &packet);
  void Clear();
  const decode::PacketInfo &PacketAt(int row) const;

private:
  std::vector<decode::PacketInfo> packets_;
};
} // namespace sniffles::ui
