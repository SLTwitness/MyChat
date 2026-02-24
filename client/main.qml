import QtQuick
import MyChat 1.0

Window {
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
            //记录当前中心点
            var centerX = mainWindow.x + mainWindow.width / 2
            var centerY = mainWindow.y + mainWindow.height / 2

            mainWindow.width=987
            mainWindow.height=753

            //修正位置，保持中心不变
            mainWindow.x = centerX - mainWindow.width / 2
            mainWindow.y = centerY - mainWindow.height / 2

            pageLoader.sourceComponent=mainPage
        }
    }
}
