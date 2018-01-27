import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
    title: qsTr("Chess")
    minimumWidth: 670
    minimumHeight: 480
    visible: true

    Item {
      id: gameBoard
      anchors.left: parent.left
      anchors.top: parent.top
      width: parent.height
      height: parent.height

      property int squareSize: width / logic.boardSize;
      property var images: [
        [
          {'imgPath' : "/images/black_pawn.svg"},
          {'imgPath' : "/images/black_knight.svg"},
          {'imgPath' : "/images/black_bishop.svg"},
          {'imgPath' : "/images/black_rook.svg"},
          {'imgPath' : "/images/black_queen.svg"},
          {'imgPath' : "/images/black_king.svg"},
        ],
        [
          {'imgPath' : "/images/white_pawn.svg"},
          {'imgPath' : "/images/white_knight.svg"},
          {'imgPath' : "/images/white_bishop.svg"},
          {'imgPath' : "/images/white_rook.svg"},
          {'imgPath' : "/images/white_queen.svg"},
          {'imgPath' : "/images/white_king.svg"},
        ]
      ]

      Image {
        source: "/images/chess_board.jpg"
        anchors.fill: parent
      }

      Repeater {
        model: logic

        Image {
          height: gameBoard.squareSize
          width: gameBoard.squareSize

          x: gameBoard.squareSize * positionX
          y: gameBoard.squareSize * positionY
          z: 0

          source: gameBoard.images[team][type].imgPath

          MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.minimumX: 0
            drag.minimumY: 0
            drag.maximumX: gameBoard.width - gameBoard.squareSize
            drag.maximumY: gameBoard.height - gameBoard.squareSize

            enabled: ingameMenu.visible ? (logic.getTeam === team ? false : true) : false

            property int startX: 0
            property int startY: 0

            onPressed: {
              startX = parent.x;
              startY = parent.y;
              parent.z = 1;
            }

            onReleased: {
              var fromX = startX / gameBoard.squareSize;
              var fromY = startY / gameBoard.squareSize;
              var toX   = (parent.x + mouseX) / gameBoard.squareSize;
              var toY   = (parent.y + mouseY) / gameBoard.squareSize;

              if (!logic.move(fromX, fromY, toX, toY)) {
                positionX = fromX;
                positionY = fromY;
              }
              parent.z = 0
            }
          }
        }
      }
    }

    Item {
      id: menuBar
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.bottom: gameBoard.bottom
      visible: true

      ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5

        ColumnLayout {
          id: mainMenu
          anchors.fill: parent
          spacing: 10

          Button {
            id: startButton
            Layout.fillWidth: true
            text: qsTr("New game")

            onClicked: {
              mainMenu.visible = false
              laodControllers.visible = false
              ingameMenu.visible = true

              logic.clear();
              logic.start();
            }
          }
          Button {
            id: loadButton
            Layout.fillWidth: true
            text: qsTr("Load")

            onClicked: {
              loadDialog.open();
            }
          }
          Button {
            id: prefButton
            Layout.fillWidth: true
            text: qsTr("Settings")
          }
          Button {
            id: quitButton
            Layout.fillWidth: true
            text: qsTr("Exit")

            onClicked: {
              Qt.quit();
            }
          }
        }

        RowLayout {
          id: laodControllers
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.bottom: parent.bottom
          visible: false

          Button {
            id: prevButton
            anchors.left: parent.left
            text: qsTr("<")
            enabled: logic.getCurrMove > 0
            Layout.maximumWidth: parent.width / 5

            onClicked: {
              logic.prev();
            }
          }
          Button {
            id: playButton
            anchors.left: prevButton.right
            anchors.right: nextButton.left
            text: qsTr("Play")
          }
          Button {
            id: nextButton
            anchors.right: parent.right
            text: qsTr(">")
            enabled: logic.getCurrMove < logic.getMovementsNum
            Layout.maximumWidth: parent.width / 5

            onClicked: {
              logic.next();
            }
          }
        }
      }
    }

    ColumnLayout {
      id: ingameMenu
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.margins: 5
      visible: false
      ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Button {
          id: saveButton
          text: qsTr("Save")
          Layout.fillWidth: true

          onClicked: {
            saveDialog.open();
          }
        }
        Button {
          id: stopButton
          text: qsTr("Stop")
          Layout.fillWidth: true

          onClicked: {
            logic.clear()

            ingameMenu.visible = false
            mainMenu.visible = true
          }
        }
      }

      Button {
        id: undoButton
        Layout.fillWidth: true
        anchors.bottom: parent.bottom
        text: qsTr("Undo")
        enabled: logic.getMovementsNum > 0

        onClicked: {
          logic.undo()
        }
      }
    }

    FileDialog {
      id: loadDialog
      title: qsTr("Please choose a file")
      nameFilters: [ "Text files (*.txt)", "All files (*)" ]
      folder: shortcuts.documents

      onAccepted: {
        if (logic.load(loadDialog.fileUrls[0])) {
          laodControllers.visible = true;
        }
      }
    }

    FileDialog {
      id: saveDialog
      title: qsTr("Save file")
      selectExisting: false
      folder: shortcuts.documents

      onAccepted: {
        logic.save(saveDialog.fileUrl);
      }
    }
}
