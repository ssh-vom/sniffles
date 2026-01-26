#pragma once

#include <cstddef>

namespace sniffles::ui {
class PacketTableModel {
public:
  std::size_t RowCount() const;
};
} // namespace sniffles::ui
