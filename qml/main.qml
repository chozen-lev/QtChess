import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
    title: qsTr("Chess")
    visible: true
    width: 800
    height: 560
    
    property int squareSize: 70

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

    Item {
        id: gameBoard
        x: 0
        y: 0
        width: logic.boardSize * squareSize
        height: logic.boardSize * squareSize

        Image {
            source: "/images/chess_board.jpg"
            height: gameBoard.height
            width: gameBoard.width
        }

        Repeater {
            model: logic

            Image {
                height: squareSize
                width: squareSize

                x: squareSize * positionX
                y: squareSize * positionY

                source: images[color][type].imgPath

                MouseArea {
                    anchors.fill: parent
                    drag.target: parent
                    drag.minimumX: 0
                    drag.maximumX: 490
                    drag.minimumY: 0
                    drag.maximumY: 490

                    enabled: true

                    property int startX: 0
                    property int startY: 0

                    onPressed: {
                        startX = parent.x;
                        startY = parent.y;
                        console.log(moves)
                    }

                    onReleased: {
                        var fromX = startX / squareSize;
                        var fromY = startY / squareSize;
                        var toX   = (parent.x + mouseX) / squareSize;
                        var toY   = (parent.y + mouseY) / squareSize;

                        if (!logic.move(fromX, fromY, toX, toY)) {
                            parent.x = startX;
                            parent.y = startY;
                        }
                    }
                }
            }
        }
    }

    ColumnLayout
    {
        anchors.top: gameBoard.top
        anchors.bottom: gameBoard.bottom
        anchors.left: gameBoard.right
        anchors.right: parent.right
        anchors.margins: 10

        ColumnLayout {
            id: mainMenu
            anchors.fill: parent

            Button {
                id: startButton
                anchors.left: parent.left
                anchors.right: parent.right

                text: "New game"

                onClicked: {
                    mainMenu.visible = false
                    loadControllers.visible = false
                    ingameMenu.visible = true

                    logic.clear()
                    logic.init()
                }
            }

            Button {
                id: loadButton
                anchors.left: parent.left
                anchors.right: parent.right

                text: "Load"

                onClicked: {
                    loadDialog.visible = true
                }
            }

            Button {
                id: exitButton
                anchors.left: parent.left
                anchors.right: parent.right

                text: "Exit"

                onClicked: {
                    Qt.quit()
                }
            }
        }

        RowLayout {
            id: loadControllers
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            visible: false

            Button {
                id: prevButton
                anchors.left: parent.left
                text: "Prev"
                enabled: false

                onClicked: {

                }
            }

            Button {
                id: nextButton
                anchors.right: parent.right
                text: "Next"
                enabled: false

                onClicked: {

                }
            }
        }

        ColumnLayout
        {
            id: ingameMenu
            anchors.fill: parent
            visible: false

            Button {
                id: saveButton
                anchors.left: parent.left
                anchors.right: parent.right

                text: "Save"

                onClicked: {
                    saveDialog.visible = true
                }
            }

            Button {
                id: stopButton
                anchors.left: parent.left
                anchors.right: parent.right

                text: "Stop"

                onClicked: {
                    ingameMenu.visible = false
                    mainMenu.visible = true
                    logic.clear()
                }
            }
        }
    }

    FileDialog {
        id: loadDialog
        title: "Please choose a file"
        folder: shortcuts.documents
        nameFilters: [ "Text files (*.txt)", "All files (*)" ]

        onAccepted: {
            if (logic.load(loadDialog.fileUrls[0])) {
                loadControllers.visible = true

                logic.clear()
                logic.init()
            }
        }

        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog {
        id: saveDialog
        title: "Save file"
        folder: shortcuts.documents
        selectExisting: false

        onAccepted: {
            logic.save(saveDialog.fileUrl)
        }

        onRejected: {
            console.log("Canceled")
        }
    }
}
