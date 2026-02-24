import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0
import QtQuick.Controls.Material

Rectangle{
    id:rightPage
    anchors.left: leftPage.right
    anchors.leftMargin: -1
    anchors.right: mainPage.right
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    color:"#EDEDED"
    radius: marginPage.radius
    border.width: 0.5
    border.color: "#D5D5D5"

    //right尖角补丁
    Rectangle{
        width: 5
        height: width
        anchors.left: rightPage.left
        anchors.top: rightPage.top
        color: rightPage.color
    }
    Rectangle{
        width: 5
        height: width
        anchors.left: rightPage.left
        anchors.bottom: rightPage.bottom
        color: rightPage.color
    }
    //right补丁结束


    Rectangle{
        id:closeRec
        anchors.right: parent.right
        anchors.rightMargin: parent.border.width
        anchors.top: parent.top
        anchors.topMargin: parent.border.width
        width: 43
        height: 31
        color:rightPage.color
        radius: rightPage.radius
        //----------------------closerec的尖角补丁
        Rectangle{
            width: 25
            height: width
            anchors.left: closeRec.left
            anchors.top: closeRec.top
            color: closeRec.color
        }
        Rectangle{
            width: 25
            height: width
            anchors.left: closeRec.left
            anchors.bottom: closeRec.bottom
            color: closeRec.color
        }
        Rectangle{
            width: 25
            height: width
            anchors.right: closeRec.right
            anchors.bottom: closeRec.bottom
            color: closeRec.color
        }
        //------------------------closerec补丁结束
        Image {
            id: closeIcon
            source: "qrc:/MyChat/Icon/LogClose.png"
            anchors.centerIn: parent
            width: 12
            height: width
            opacity: 0.1                    //越透明coloroverlay才越能生效
            z:3
        }
        ColorOverlay{
            id:closeIcon_mask
            source: closeIcon
            anchors.fill: closeIcon
            color: "#484848"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                closeRec.color="#ED4C4C"
                closeIcon_mask.color="white"
            }
            onExited: {
                closeRec.color=rightPage.color
                closeIcon_mask.color="#484848"
            }
            onClicked: {
                Qt.quit()
            }
        }
    }

    Rectangle{
        id:maxRec
        anchors.top: parent.top
        anchors.right: closeRec.left
        width: closeRec.width
        height: closeRec.height
        color: rightPage.color
        anchors.topMargin: parent.border.width
        property bool ismax: false
        Image {
            id: maxIcon
            source: "qrc:/MyChat/Icon/Maximize.png"
            anchors.centerIn: parent
            width: 11
            height: width
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                maxRec.color="#E1E1E1"
            }
            onExited: {
                maxRec.color=rightPage.color
            }
            onClicked: {
                parent.ismax=!parent.ismax
                if(parent.ismax){
                    mainWindow.showMaximized()
                }
                else{
                    mainWindow.showNormal()
                }
            }
        }
    }

    Rectangle{
        id:minRec
        anchors.top: parent.top
        anchors.right: maxRec.left
        width: closeRec.width
        height: closeRec.height
        color: rightPage.color
        anchors.topMargin: parent.border.width
        Image {
            id: minIcon
            source: "qrc:/MyChat/Icon/Minimize.png"
            anchors.centerIn: parent
            width: 10
            height: 6
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                minRec.color="#E1E1E1"
            }
            onExited: {
                minRec.color=rightPage.color
            }
            onClicked: {
                mainWindow.showMinimized()
            }
        }
    }

    Rectangle{
        id:topRec
        anchors.top: parent.top
        anchors.right: minRec.left
        width: closeRec.width
        height: closeRec.height
        color: rightPage.color
        anchors.topMargin: parent.border.width
        property bool istop: false
        Image {
            id: topIcon
            source: "qrc:/MyChat/Icon/Top.png"
            anchors.centerIn: parent
            width: 9
            height: 11
        }
        ColorOverlay{
            id:topIcon_mask
            source: topIcon
            anchors.fill: topIcon
            color: "#484848"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                topRec.color="#E1E1E1"
            }
            onExited: {
                topRec.color=rightPage.color
            }
            onClicked: {
                if(!parent.istop){
                    topIcon_mask.color="#07C160"
                    mainWindow.flags = Qt.Window | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
                }
                else{
                    topIcon_mask.color="#484848"
                    mainWindow.flags = Qt.Window | Qt.FramelessWindowHint
                }
                mainWindow.show()
                parent.istop=!parent.istop
            }
        }
    }


    /*----------------------------------聊天界面----------------------------------*/
    Item{
        id:chatRec
        anchors.fill: parent
        Text {
            id: chatName
            text: "Test"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 28
            anchors.leftMargin: 21
            font.pixelSize: 18
        }
        Rectangle{                                        //分割线
            id:dividRec
            height: 0.7
            width: parent.width
            color:"#D5D5D5"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: chatName.bottom
            anchors.topMargin: 16
        }
        Rectangle{
            id:chatInfoRec
            anchors.bottom: dividRec.top
            anchors.bottomMargin: 3
            anchors.right: parent.right
            anchors.rightMargin: 19
            width: 32
            height: width
            color:"transparent"
            radius: 3
            Image {
                id: chatInfo
                source: "qrc:/MyChat/Icon/ChatInfo.png"
                width: 17
                height: 10
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    chatInfoRec.color="#E1E1E1"
                }
                onExited: {
                    chatInfoRec.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatVideoRec
            anchors.verticalCenter: chatInfoRec.verticalCenter
            anchors.right: chatInfoRec.left
            anchors.rightMargin: 7
            width: 32
            height: width
            color:"transparent"
            radius: 3
            Image {
                id: chatVideo
                source: "qrc:/MyChat/Icon/ChatVideo.png"
                width: 21
                height: 19
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    chatVideoRec.color="#E1E1E1"
                }
                onExited: {
                    chatVideoRec.color="transparent"
                }
            }
        }

        Rectangle{                                        //分割线
            id:dividRec1
            height: 1
            width: parent.width
            color:"#D5D5D5"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 150
        }
        Rectangle{
            id:chatEmojiRec
            width: 30
            height: width
            color:"transparent"
            anchors.left: parent.left
            anchors.top: dividRec1.top
            anchors.leftMargin: 13
            anchors.topMargin: 9
            radius: 3
            Image {
                id: chatEmoji
                source: "qrc:/MyChat/Icon/ChatEmoji.png"
                anchors.centerIn: parent
                width: 20
                height: width
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatCollectRec
            width: 30
            height: width
            color:"transparent"
            anchors.left: chatEmojiRec.right
            anchors.verticalCenter: chatEmojiRec.verticalCenter
            anchors.leftMargin: 10
            radius: 3
            Image {
                id: chatCollect
                source: "qrc:/MyChat/Icon/ChatCollect.png"
                anchors.centerIn: parent
                width: 20
                height: 22
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatFileRec
            width: 30
            height: width
            color:"transparent"
            anchors.left: chatCollectRec.right
            anchors.verticalCenter: chatEmojiRec.verticalCenter
            anchors.leftMargin: 10
            radius: 3
            Image {
                id: chatFile
                source: "qrc:/MyChat/Icon/ChatFile.png"
                anchors.centerIn: parent
                width: 19
                height: 17
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatScreenShotRec
            width: 30
            height: width
            color:"transparent"
            anchors.left: chatFileRec.right
            anchors.verticalCenter: chatEmojiRec.verticalCenter
            anchors.leftMargin: 10
            radius: 3
            Image {
                id: chatScreenShot
                source: "qrc:/MyChat/Icon/ChatScreenShot.png"
                anchors.centerIn: parent
                width: 20
                height: 17
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatMoreRec
            width: 15
            height: chatScreenShotRec.height
            color:"transparent"
            anchors.left: chatScreenShotRec.right
            anchors.verticalCenter: chatEmojiRec.verticalCenter
            radius: 2
            Image {
                id: chatMore
                source: "qrc:/MyChat/Icon/ChatMore.png"
                anchors.centerIn: parent
                width: 8
                height: 5
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatRecordRec
            width: 30
            height: width
            color:"transparent"
            anchors.left: chatMoreRec.right
            anchors.verticalCenter: chatEmojiRec.verticalCenter
            anchors.leftMargin: 10
            radius: 3
            Image {
                id: chatRecord
                source: "qrc:/MyChat/Icon/ChatRecord.png"
                anchors.centerIn: parent
                width: 19
                height: 18
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }
        Rectangle{
            id:chatVoiceRec
            width: 30
            height: width
            color:"transparent"
            anchors.left: chatRecordRec.right
            anchors.verticalCenter: chatEmojiRec.verticalCenter
            anchors.leftMargin: 10
            radius: 3
            Image {
                id: chatVoice
                source: "qrc:/MyChat/Icon/ChatVoice.png"
                anchors.centerIn: parent
                width: 16
                height: 20
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#E1E1E1"
                }
                onExited: {
                    parent.color="transparent"
                }
            }
        }

        TextArea{
            id:txtRec
            anchors.top:chatEmojiRec.bottom
            anchors.topMargin:22
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width-4
            height: 90
            wrapMode: TextArea.Wrap
            Material.accent: "#07C160"
            font.pixelSize: 14
            font.weight: 200
            background: Rectangle{
                color:"transparent"
                border.color: "transparent"
            }
        }

        Rectangle{
            id:sendRec
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.bottomMargin: 15
            anchors.rightMargin: 10
            width: 90
            height: 32
            property bool ishover: false
            color:txtRec.text===""?"#E1E1E1":ishover?"#06B75B":"#07C160"
            radius: 4
            Text {
                id: sendtxt
                text: "发送(S)"
                color:txtRec.text===""?"#9D9D9D":"white"
                font.pixelSize: 14
                font.weight: 400
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.ishover=true
                }
                onExited: {
                    parent.ishover=false
                }
                onClicked: {
                    txtRec.text=""
                }
            }
        }
    }
    /*----------------------------------聊天界面----------------------------------*/


    Image {
        id: myChatIcon
        source: "qrc:/MyChat/Icon/MyChat.png"
        anchors.centerIn: parent
        width: 95
        height: 75
        visible: false
    }

    Connections{
        target: AppState
        function onShowPage(){
            myChatIcon.visible=true
            chatRec.visible=false
        }
        function onShowHome(){
            myChatIcon.visible=true
            chatRec.visible=false
        }
        function onSearchFocus(){
            txtRec.focus=false
        }
        function onShowChatPage(){
            myChatIcon.visible=true
        }
        function onShowChat(name){
            myChatIcon.visible=false
            chatRec.visible=true
            chatName.text=name
        }
    }
}
