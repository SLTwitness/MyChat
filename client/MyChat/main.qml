import QtQuick
import MyChat 1.0
import QtQuick.Controls

ApplicationWindow {
    id:mainWindow
    width:280
    height:380
    visible: true
    title: qsTr("Hello World")
    flags: Qt.FramelessWindowHint|Qt.Window
    color:"transparent"

    Component{
        id:loginPage
        LoginPage{}
    }

    Component{
        id:mainPage
        MainPage{}
    }

    Loader{
        id:pageLoader
        anchors.fill: parent
        sourceComponent:loginPage
    }

    Connections{
        target:AppState
        function onLoginSuccess(){
            mainWindow.width=987
            mainWindow.height=753

            mainWindow.x = (Screen.width - mainWindow.width) / 2
            mainWindow.y = (Screen.height - mainWindow.height) / 2

            pageLoader.sourceComponent=mainPage
        }
    }
}
