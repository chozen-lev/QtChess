import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
    title: qsTr("Chess")
    minimumWidth: 800
    minimumHeight: 600
    maximumWidth: 800
    maximumHeight: 600
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
          {'imgPath' : "/images/white_pawn.svg"},
          {'imgPath' : "/images/white_knight.svg"},
          {'imgPath' : "/images/white_bishop.svg"},
          {'imgPath' : "/images/white_rook.svg"},
          {'imgPath' : "/images/white_queen.svg"},
          {'imgPath' : "/images/white_king.svg"},
        ],
        [
          {'imgPath' : "/images/black_pawn.svg"},
          {'imgPath' : "/images/black_knight.svg"},
          {'imgPath' : "/images/black_bishop.svg"},
          {'imgPath' : "/images/black_rook.svg"},
          {'imgPath' : "/images/black_queen.svg"},
          {'imgPath' : "/images/black_king.svg"},
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

          source: gameBoard.images[team][type].imgPath

          MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.minimumX: 0
            drag.minimumY: 0
            drag.maximumX: gameBoard.width - gameBoard.squareSize
            drag.maximumY: gameBoard.height - gameBoard.squareSize

            property int startX: 0
            property int startY: 0

            onPressed: {
              startX = parent.x;
              startY = parent.y;
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
        anchors.margins: 10

        ColumnLayout {
          id: mainMenu
          anchors.fill: parent
          spacing: 10

          Button {
            id: startButton
            Layout.fillWidth: true
            text: "New game"

            onClicked: {
              mainMenu.visible = false
              ingameMenu.visible = true

              logic.clear();
              logic.start();
            }
          }
          Button {
            id: loadButton
            Layout.fillWidth: true
            text: "Load"

            onClicked: {
              loadDialog.open();
            }
          }
          Button {
            id: quitButton
            Layout.fillWidth: true
            text: "Exit"

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
            text: "Prev"
          }
          Button {
            id: nextButton
            anchors.right: parent.right
            text: "Next"
          }
        }
      }
    }

    ColumnLayout {
      id: ingameMenu
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.leftMargin: 10
      anchors.rightMargin: 10
      anchors.topMargin: 10
      visible: false

      Button {
        id: saveButton
        text: "Save"
        anchors.left: parent.left
        anchors.right: parent.right

        onClicked: {
          saveDialog.open();
        }
      }
      Button {
        id: stopButton
        text: "Stop"
        anchors.left: parent.left
        anchors.right: parent.right

        onClicked: {
          logic.clear()

          ingameMenu.visible = false
          mainMenu.visible = true
        }
      }
    }

    FileDialog {
      id: loadDialog
      title: "Please choose a file"
      nameFilters: [ "Text files (*.txt)", "All files (*)" ]
      folder: shortcuts.documents

      onAccepted: {
        laodControllers.visible = true
      }
    }

    FileDialog {
      id: saveDialog
      title: "Save file"
      selectExisting: false
      folder: shortcuts.documents
    }
}
