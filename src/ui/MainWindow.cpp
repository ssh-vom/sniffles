#include "ui/MainWindow.h"

#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include <cassert>
#include <iomanip>
#include <sstream>

namespace {
QString FormatDevice(const sniffles::capture::DeviceInfo &device) {
  QString text = QString::fromStdString(device.name + " - " + device.description);
  if (!device.ipv4_address.empty()) {
    text += " [" + QString::fromStdString(device.ipv4_address) + "]";
  }
  if (device.is_loopback) {
    text += " [loopback]";
  }
  return text;
}

QString FormatBytes(const std::vector<uint8_t> &bytes) {
  std::ostringstream out;
  out << std::hex << std::setfill('0');
  for (size_t i = 0; i < bytes.size(); ++i) {
    if (i > 0) {
      if (i % 16 == 0) {
        out << '\n';
      } else {
        out << ' ';
      }
    }
    out << std::setw(2) << static_cast<int>(bytes[i]);
  }
  return QString::fromStdString(out.str());
}
} // namespace

namespace sniffles::ui {
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      capture_service_(std::make_unique<capture::CaptureService>()) {
  SetupUI();
  PopulateInterfaces();
  RefreshStats();
}

MainWindow::~MainWindow() {
  capture_service_->Stop();
}

void MainWindow::SetupUI() {
  auto *central = new QWidget(this);
  auto *root = new QVBoxLayout(central);
  auto *controls = new QHBoxLayout();

  interface_combo_ = new QComboBox(central);
  filter_edit_ = new QLineEdit(central);
  start_button_ = new QPushButton("Start", central);
  stop_button_ = new QPushButton("Stop", central);
  clear_button_ = new QPushButton("Clear", central);
  packet_table_ = new QTableView(central);
  details_view_ = new QPlainTextEdit(central);
  status_label_ = new QLabel(central);
  interface_label_ = new QLabel(central);
  stats_label_ = new QLabel(central);
  poll_timer_ = new QTimer(this);
  packet_model_ = new PacketTableModel(this);

  filter_edit_->setPlaceholderText("tcp or udp or port 53");
  details_view_->setReadOnly(true);
  stop_button_->setEnabled(false);

  controls->addWidget(new QLabel("Interface", central));
  controls->addWidget(interface_combo_, 2);
  controls->addWidget(new QLabel("Filter", central));
  controls->addWidget(filter_edit_, 2);
  controls->addWidget(start_button_);
  controls->addWidget(stop_button_);
  controls->addWidget(clear_button_);

  packet_table_->setModel(packet_model_);
  packet_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  packet_table_->setSelectionMode(QAbstractItemView::SingleSelection);
  packet_table_->setSortingEnabled(true);
  packet_table_->horizontalHeader()->setStretchLastSection(true);
  packet_table_->verticalHeader()->setVisible(false);

  auto *splitter = new QSplitter(Qt::Vertical, central);
  splitter->addWidget(packet_table_);
  splitter->addWidget(details_view_);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 2);

  root->addLayout(controls);
  root->addWidget(status_label_);
  root->addWidget(interface_label_);
  root->addWidget(stats_label_);
  root->addWidget(splitter, 1);

  setCentralWidget(central);
  resize(1200, 760);
  setWindowTitle("sniffles");

  connect(start_button_, &QPushButton::clicked, this, &MainWindow::OnStartCapture);
  connect(stop_button_, &QPushButton::clicked, this, &MainWindow::OnStopCapture);
  connect(clear_button_, &QPushButton::clicked, this, &MainWindow::OnClearPackets);
  connect(poll_timer_, &QTimer::timeout, this, &MainWindow::OnPollQueue);
  connect(packet_table_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &MainWindow::OnPacketSelected);
}

void MainWindow::PopulateInterfaces() {
  std::string error;
  if (!capture::DeviceEnumerator::ListDevices(devices_, error)) {
    status_label_->setText(QString::fromStdString(error));
    start_button_->setEnabled(false);
    return;
  }

  for (const capture::DeviceInfo &device : devices_) {
    interface_combo_->addItem(QString::fromStdString(device.name));
  }

  assert(!devices_.empty());
  interface_combo_->setCurrentIndex(0);
  interface_label_->setText(FormatDevice(devices_.front()));

  connect(interface_combo_, qOverload<int>(&QComboBox::currentIndexChanged), this,
          [this](int index) { interface_label_->setText(FormatDevice(devices_.at(index))); });
}

void MainWindow::OnStartCapture() {
  const int index = interface_combo_->currentIndex();
  assert(index >= 0);

  packet_model_->Clear();
  details_view_->clear();

  capture::CaptureRequest request;
  request.device_name = devices_.at(index).name;
  request.filter_expression = filter_edit_->text().trimmed().toStdString();

  if (!capture_service_->Start(request)) {
    status_label_->setText("Failed to start capture. Check permissions or filter.");
    return;
  }

  capture_started_at_ = std::chrono::steady_clock::now();
  poll_timer_->start(100);
  status_label_->setText("Capturing packets");
  start_button_->setEnabled(false);
  stop_button_->setEnabled(true);
  interface_combo_->setEnabled(false);
  filter_edit_->setEnabled(false);
  RefreshStats();
}

void MainWindow::OnStopCapture() {
  OnPollQueue();
  capture_service_->Stop();
  poll_timer_->stop();
  status_label_->setText("Capture stopped");
  start_button_->setEnabled(true);
  stop_button_->setEnabled(false);
  interface_combo_->setEnabled(true);
  filter_edit_->setEnabled(true);
  RefreshStats();
}

void MainWindow::OnClearPackets() {
  packet_model_->Clear();
  details_view_->clear();
  RefreshStats();
}

void MainWindow::OnPollQueue() {
  decode::PacketInfo packet;
  while (capture_service_->GetPacketQueue().TryPop(packet)) {
    packet_model_->AddPacket(packet);
  }

  if (packet_model_->rowCount() > 0) {
    packet_table_->scrollToBottom();
  }

  RefreshStats();
}

void MainWindow::OnPacketSelected() {
  const QModelIndex index = packet_table_->currentIndex();
  if (!index.isValid()) {
    details_view_->clear();
    return;
  }

  const decode::PacketInfo &packet = packet_model_->PacketAt(index.row());
  QString text;
  text += "Summary: " + QString::fromStdString(packet.summary) + "\n";
  text += "Protocol: " + QString::fromStdString(packet.protocol_name) + "\n";
  text += "Source: " + QString::fromStdString(packet.flow.src) + "\n";
  text += "Destination: " + QString::fromStdString(packet.flow.dst) + "\n";
  text += "Length: " + QString::number(static_cast<qulonglong>(packet.length)) + "\n";
  text += "Captured: " + QString::number(static_cast<qulonglong>(packet.captured_length)) + "\n\n";
  text += FormatBytes(packet.raw_data);
  details_view_->setPlainText(text);
}

void MainWindow::RefreshStats() {
  if (!capture_service_->IsRunning()) {
    stats_label_->setText(
        QString("Packets %1 | Rate 0 pkt/s | Drops 0 | Iface drops 0")
            .arg(packet_model_->rowCount()));
    return;
  }

  const capture::CaptureStats stats = capture_service_->GetStats();
  const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now() - capture_started_at_);
  const double seconds = elapsed.count() == 0 ? 1.0 : static_cast<double>(elapsed.count());
  const double rate = static_cast<double>(stats.packets_received) / seconds;
  stats_label_->setText(
      QString("Packets %1 | Visible %2 | Rate %3 pkt/s | Drops %4 | Iface drops %5")
          .arg(stats.packets_received)
          .arg(packet_model_->rowCount())
          .arg(rate, 0, 'f', 1)
          .arg(stats.packets_dropped)
          .arg(stats.packets_dropped_by_interface));
}
} // namespace sniffles::ui
