#pragma once

#include "capture/CaptureService.h"
#include <QMainWindow>
#include <QTimer>
#include <memory>
class QTableWidget;
class QComboBox;
class QPushButton;
class QLabel;

namespace sniffles::ui {
class MainWindow : public QMainWindow {
  Q_OBJECT; // Needed for Qt signals/slots

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void OnStartCapture();
  void OnStopCapture();
  void OnPollQueue();

private:
  void SetupUI();
  void PopulateInterfaces();
  void AddPacketToTable(const decode::PacketInfo &info);

  QComboBox *interface_combo_ = nullptr;
  QPushButton *start_button = nullptr;
  QPushButton *stop_button = nullptr;
  QTableWidget *packet_table = nullptr;
  QLabel *status_label_ = nullptr;

  QTimer *poll_timer_ = nullptr;

  std::unique_ptr<capture::CaptureService> capture_service_;
};
} // namespace sniffles::ui
