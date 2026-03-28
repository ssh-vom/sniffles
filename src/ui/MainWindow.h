#pragma once

#include "capture/CaptureService.h"
#include "capture/DeviceEnumerator.h"
#include "ui/models/PacketTableModel.h"

#include <QMainWindow>
#include <QTimer>

#include <chrono>
#include <memory>
#include <vector>

class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTableView;

namespace sniffles::ui {
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void OnStartCapture();
  void OnStopCapture();
  void OnClearPackets();
  void OnPollQueue();
  void OnPacketSelected();

private:
  void SetupUI();
  void PopulateInterfaces();
  void RefreshStats();

  QComboBox *interface_combo_ = nullptr;
  QLineEdit *filter_edit_ = nullptr;
  QPushButton *start_button_ = nullptr;
  QPushButton *stop_button_ = nullptr;
  QPushButton *clear_button_ = nullptr;
  QTableView *packet_table_ = nullptr;
  QPlainTextEdit *details_view_ = nullptr;
  QLabel *status_label_ = nullptr;
  QLabel *interface_label_ = nullptr;
  QLabel *stats_label_ = nullptr;
  QTimer *poll_timer_ = nullptr;
  PacketTableModel *packet_model_ = nullptr;
  std::vector<capture::DeviceInfo> devices_;
  std::chrono::steady_clock::time_point capture_started_at_;
  std::unique_ptr<capture::CaptureService> capture_service_;
};
} // namespace sniffles::ui
