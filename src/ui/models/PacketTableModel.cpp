#include "ui/models/PacketTableModel.h"

#include <QTime>

namespace {
QString FormatEndpoint(const sniffles::decode::FlowKey &flow, bool source) {
  const std::string &host = source ? flow.src : flow.dst;
  const std::uint16_t port = source ? flow.src_port : flow.dst_port;
  if (port == 0) {
    return QString::fromStdString(host);
  }
  return QString::fromStdString(host + ":" + std::to_string(port));
}
} // namespace

namespace sniffles::ui {
PacketTableModel::PacketTableModel(QObject *parent) : QAbstractTableModel(parent) {}

int PacketTableModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return static_cast<int>(packets_.size());
}

int PacketTableModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return 6;
}

QVariant PacketTableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || role != Qt::DisplayRole) {
    return {};
  }

  const decode::PacketInfo &packet = packets_.at(index.row());
  switch (index.column()) {
  case 0:
    return QTime::fromMSecsSinceStartOfDay(0)
        .addSecs(static_cast<int>(std::chrono::system_clock::to_time_t(packet.timestamp) % 86400))
        .toString("HH:mm:ss");
  case 1:
    return FormatEndpoint(packet.flow, true);
  case 2:
    return FormatEndpoint(packet.flow, false);
  case 3:
    return QString::fromStdString(packet.protocol_name);
  case 4:
    return static_cast<qulonglong>(packet.length);
  case 5:
    return QString::fromStdString(packet.summary);
  default:
    return {};
  }
}

QVariant PacketTableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
  case 0:
    return "Time";
  case 1:
    return "Source";
  case 2:
    return "Destination";
  case 3:
    return "Protocol";
  case 4:
    return "Length";
  case 5:
    return "Summary";
  default:
    return {};
  }
}

void PacketTableModel::AddPacket(const decode::PacketInfo &packet) {
  const int row = static_cast<int>(packets_.size());
  beginInsertRows(QModelIndex(), row, row);
  packets_.push_back(packet);
  endInsertRows();
}

void PacketTableModel::Clear() {
  beginResetModel();
  packets_.clear();
  endResetModel();
}

const decode::PacketInfo &PacketTableModel::PacketAt(int row) const {
  return packets_.at(row);
}
} // namespace sniffles::ui
