import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0
import QtQuick.Controls.Material
import User 1.0
import Tcp 1.0


Item{
    id:friendRec
    anchors.fill: parent
    visible: false

    property string toName: ""
    property int toSex: 1
    property string toImg: ""
    property int toUid: -1

    Image {
        id: friendImg
        source: friendRec.toImg
        anchors.top: parent.top
        anchors.topMargin: 90
        anchors.left: friendDivid1.left
        width: 60
        height: width
        layer.enabled: true
        layer.smooth: true
        layer.effect: OpacityMask{
            maskSource: Rectangle{
                radius: 4
                width: friendImg.width
                height: friendImg.height
                color:"white"
            }
        }
    }
    Text {
        id: friendName
        text: friendRec.toName===""?"test":friendRec.toName
        font.pixelSize: 15
        font.weight: 400
        color:"black"
        anchors.left: friendImg.right
        anchors.leftMargin: 17
        anchors.top: friendImg.top
    }
    Image {
        id: friendSex
        source: friendRec.toSex===1?"qrc:/MyChat/Icon/ManIcon.png":"qrc:/MyChat/Icon/WomanIcon.png"
        anchors.bottom: friendName.bottom
        anchors.bottomMargin: 2.5
        anchors.left: friendName.right
        anchors.leftMargin: 5
        width: 12.5
        height: width
    }
    //分割线1
    Rectangle{
        id:friendDivid1
        height: 0.7
        width: 320
        color:"#D5D5D5"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: friendImg.bottom
        anchors.topMargin: 18
    }

    Text {
        id: noteTxt
        text: "备注"
        font.pixelSize: 14
        font.weight: 400
        color:"#9F9FA6"
        anchors.left: friendDivid1.left
        anchors.top: friendDivid1.bottom
        anchors.topMargin: 17
    }
    Text {
        id: noteContentTxt
        text: friendRec.toName===""?"test":friendRec.toName
        font.pixelSize: 14
        font.weight: 200
        color:"black"
        anchors.left: noteTxt.right
        anchors.leftMargin: 50
        anchors.bottom: noteTxt.bottom
    }
    //分割线2
    Rectangle{
        id:friendDivid2
        height: 0.7
        width: friendDivid1.width
        color:"#D5D5D5"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: noteContentTxt.bottom
        anchors.topMargin: 18
    }

    Text {
        id: friendFromTxt
        text: "来源"
        font.pixelSize: 14
        font.weight: 200
        color:"#9F9FA6"
        anchors.left: friendDivid2.left
        anchors.top: friendDivid2.bottom
        anchors.topMargin: 17
    }
    Text {
        id: friendSourceTxt
        text: "通过搜索添加"
        font.pixelSize: 14
        font.weight:400
        color:"black"
        anchors.left: friendFromTxt.right
        anchors.leftMargin: 50
        anchors.bottom: friendFromTxt.bottom
    }
    //分割线3
    Rectangle{
        id:friendDivid3
        height: 0.7
        width: friendDivid1.width
        color:"#D5D5D5"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: friendFromTxt.bottom
        anchors.topMargin: 17
    }

    // 消息、语音、视频 按钮
    Rectangle{
        id:voiceChatRec
        color:"transparent"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:friendDivid3.bottom
        anchors.topMargin: 22
        width: 55
        height: width
        radius: 5
        Image {
            id: voiceChatIcon
            source: "qrc:/MyChat/Icon/VoiceChatIcon.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top:parent.top
            anchors.topMargin: 5
            width: 20
            height: width
        }
        Text {
            id: voiceChatTxt
            text: "语音聊天"
            font.pixelSize: 12
            font.weight:200
            color:"#4B6392"
            anchors.horizontalCenter: voiceChatIcon.horizontalCenter
            anchors.top:voiceChatIcon.bottom
            anchors.topMargin: 7
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
                voiceChatRec.color="#D3D3D3"
            }
            onExited: {
                voiceChatRec.color="transparent"
            }
        }
    }
    Rectangle{
        id:msgChatRec
        color:"transparent"
        anchors.verticalCenter: voiceChatRec.verticalCenter
        anchors.right:voiceChatRec.left
        anchors.rightMargin: 40
        width: 55
        height: width
        radius: 5
        Image {
            id: msgChatIcon
            source: "qrc:/MyChat/Icon/MsgChatIcon.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top:parent.top
            anchors.topMargin: 5
            width: 20
            height: width
        }
        Text {
            id: msgChatTxt
            text: "发消息"
            font.pixelSize: 12
            font.weight:200
            color:"#4B6392"
            anchors.horizontalCenter: msgChatIcon.horizontalCenter
            anchors.top:msgChatIcon.bottom
            anchors.topMargin: 7
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
                msgChatRec.color="#D3D3D3"
            }
            onExited: {
                msgChatRec.color="transparent"
            }
            onClicked: {
                AppState.showChat(friendRec.toUid,friendRec.toName,friendRec.toImg)
            }
        }
    }
    Rectangle{
        id:videoChatRec
        color:"transparent"
        anchors.verticalCenter: voiceChatRec.verticalCenter
        anchors.left:voiceChatRec.right
        anchors.leftMargin: 40
        width: 55
        height: width
        radius: 5
        Image {
            id: videoChatIcon
            source: "qrc:/MyChat/Icon/VideoChatIcon.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top:parent.top
            anchors.topMargin: 5
            width: 20
            height: width
        }
        Text {
            id: videoChatTxt
            text: "视频聊天"
            font.pixelSize: 12
            font.weight:200
            color:"#4B6392"
            anchors.horizontalCenter: videoChatIcon.horizontalCenter
            anchors.top:videoChatIcon.bottom
            anchors.topMargin: 7
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
                videoChatRec.color="#D3D3D3"
            }
            onExited: {
                videoChatRec.color="transparent"
            }
        }
    }
}
