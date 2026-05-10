import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0
import QtQuick.Controls.Material
import User 1.0
import Tcp 1.0


Item{
    id:chatRec
    anchors.fill: parent

    property alias txtFocus: txtRec.focus

    property int chatUid:-1
    property string chatNameTxt: ""
    property string chatImg:""

    property var chatMsgListModel: []

    Text {
        id: chatName
        text: chatRec.chatNameTxt
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


    /*---------------------------------------聊天内容界面------------------------------------*/
    Item {
        id: chatMsgItem
        anchors.top: dividRec.bottom
        anchors.bottom: dividRec1.top
        anchors.left: parent.left
        anchors.right: parent.right

        ListView{
            id:chatMsgList
            anchors.fill: parent
            model:chatRec.chatMsgListModel

            delegate: Item{
                anchors.right: modelData.isMe?parent.right:undefined
                anchors.left: modelData.isMe?undefined:parent.left
                anchors.rightMargin: 15
                anchors.leftMargin: 15
                width: 450
                height: msgRec.height+20
                Image {
                    id:userImg
                    source: modelData.img
                    anchors.right: modelData.isMe?parent.right:undefined
                    anchors.rightMargin: 5
                    anchors.left: modelData.isMe?undefined:parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    width: 38
                    height: width
                    layer.enabled: true
                    layer.smooth: true
                    layer.effect: OpacityMask{
                        maskSource: Rectangle{
                            radius: 4
                            width: userImg.width
                            height: userImg.height
                            color:"white"
                        }
                    }
                }
                Rectangle{
                    id:msgRec
                    anchors.right: modelData.isMe?userImg.left:undefined
                    anchors.left: modelData.isMe?undefined:userImg.right
                    anchors.rightMargin: 12
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    color:modelData.isMe?"#9DF29F":"#D6D6D8"
                    width: Math.min(msgTxt.implicitWidth+20, 380)
                    height: msgTxt.contentHeight+16
                    radius: 4
                    Text {
                        id:msgTxt
                        text: modelData.msg
                        wrapMode: Text.WrapAnywhere
                        anchors.centerIn: parent
                        font.pixelSize: 14
                        font.weight: 400
                        width: parent.width - 20
                        color:"black"
                    }
                }
                Canvas{
                    width: 7
                    height: 11
                    anchors.left: modelData.isMe?msgRec.right:undefined
                    anchors.right: modelData.isMe?undefined:msgRec.left
                    anchors.rightMargin: -1
                    anchors.leftMargin: -1
                    anchors.verticalCenter: msgRec.verticalCenter
                    onPaint: {
                        //拿到画笔
                        var ctx = getContext("2d")
                        //清空画布
                        ctx.clearRect(0,0,width,height)
                        //设置填充颜色
                        ctx.fillStyle = modelData.isMe?"#9DF29F":"#D6D6D8"
                        //开始新的路径
                        ctx.beginPath()

                        if(modelData.isMe){
                            ctx.moveTo(0,0)
                            ctx.lineTo(width,height/2)
                            ctx.lineTo(0,height)
                        }
                        else{
                            //将笔尖移动到此坐标，不画线
                            ctx.moveTo(width,0)
                            //从当前点画到新点 (width,0)->(width,height)
                            ctx.lineTo(width,height)
                            ctx.lineTo(0,height/2)
                        }

                        //自动闭合路径
                        ctx.closePath()
                        //用fillStyle填充
                        ctx.fill()
                    }
                }
            }

            Connections {
                target: UserMgr
                function onChatListChanged(uid) {
                    if (uid === chatRec.chatUid) {
                        chatRec.chatMsgListModel = UserMgr.getChatList(chatRec.chatUid)
                        chatMsgList.positionViewAtEnd()
                    }
                }
            }
        }
    }
    /*---------------------------------------聊天内容界面------------------------------------*/


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
                UserMgr.addChatMsg(chatRec.chatUid,txtRec.text,true,UserMgr.getIcon())

                TcpMgr.sendChatMsg(UserMgr.getUid(),chatRec.chatUid,txtRec.text)
                txtRec.text=""
                chatRec.chatMsgListModel = UserMgr.getChatList(chatRec.chatUid)
                chatMsgList.positionViewAtEnd()
            }
        }
    }

    onChatUidChanged: {
        chatMsgListModel = UserMgr.getChatList(chatUid)
        chatMsgList.positionViewAtEnd()
    }
}
