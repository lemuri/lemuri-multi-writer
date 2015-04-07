import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0

import DisksManager 1.0
//import UDisks 2.0

ApplicationWindow {
    title: "Disks Manager"
    visible: true
    width: 600
    height: 400
    minimumHeight: 400
    minimumWidth: 570

    SystemPalette {id: syspal}
    color: syspal.window

    ColumnLayout {

    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            TableView {
                Layout.fillWidth: true
                model: DevicesModel {
                    id: devicesModel
                }
                TableViewColumn{ role: "roleName" ; title: "Device" }

                onActivated: {
                    driveLayout.deviceName = devicesModel.getDeviceName(row)
                    driveLayout.drive = devicesModel.getDrive(row)
//                    driveModel.text = drive.model
                }
            }
        }
    }
}
