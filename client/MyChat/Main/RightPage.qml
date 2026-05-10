import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0
import QtQuick.Controls.Material
import User 1.0
import Tcp 1.0

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
    ChatRight{
        id:chatRec
        anchors.fill: parent
    }
    /*----------------------------------聊天界面----------------------------------*/


    /*----------------------------------添加好友界面----------------------------------*/
    AddFriendRight{
        id:newFriendRec
    }
    /*----------------------------------添加好友界面----------------------------------*/


    /*----------------------------------好友简介界面----------------------------------*/
    ContactRight{
        id:friendRec
    }
    /*----------------------------------好友简介界面----------------------------------*/

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
            newFriendRec.visible=false
            friendRec.visible=false
        }
        function onShowHome(){
            myChatIcon.visible=true
            chatRec.visible=false
            newFriendRec.visible=false
            friendRec.visible=false
        }
        function onSearchFocus(){
            chatRec.txtFocus=false
        }
        function onShowChatPage(){
            myChatIcon.visible=true
        }

        //聊天列表选中
        function onShowChat(uid,name,icon){
            myChatIcon.visible=false
            chatRec.visible=true
            newFriendRec.visible=false
            friendRec.visible=false

            //获取对方信息
            chatRec.chatUid=uid
            chatRec.chatNameTxt=name
            chatRec.chatImg=icon
            chatRec.chatMsgListModel=UserMgr.getChatList(uid)
        }

        //好友申请列表选中
        function onShowFriendApply(uid,name,sex,img,msg){
            myChatIcon.visible=false
            newFriendRec.visible=true
            friendRec.visible=false
            newFriendRec.toName=name
            newFriendRec.toSex=sex
            newFriendRec.toImg=img
            newFriendRec.toUid=uid

            //是否已验证通过？
            newFriendRec.applyAgreeTxtVisible=msg===0?false:true
            newFriendRec.agreeApplyRecVisible=msg===0?true:false
        }

        function onShowFriend(uid,name,sex,img){
            myChatIcon.visible=false
            newFriendRec.visible=false
            friendRec.visible=true
            chatRec.visible=false
            friendRec.toName=name
            friendRec.toSex=sex
            friendRec.toImg=img
            friendRec.toUid=uid
        }
    }
}
