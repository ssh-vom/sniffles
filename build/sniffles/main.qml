import QtQuick
import QtQuick.Controls
import QtCharts

Window {
    width: 900
    height: 600
    visible: true
    title: "Sniffer Charts"

    ChartView {
        anchors.fill: parent
        antialiasing: true
        legend.alignment: Qt.AlignBottom

        PieSeries {
            id: pieSeries
            PieSlice { label: "TCP"; value: 45.0 }
            PieSlice { label: "UDP"; value: 25.0 }
            PieSlice { label: "ICMP"; value: 10.0 }
            PieSlice { label: "Other"; value: 20.0 }
        }
    }
}
