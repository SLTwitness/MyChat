import QtQuick 2.15
import QtQuick.Controls.Material
import MyChat 1.0
import Qt5Compat.GraphicalEffects
import Tcp 1.0
import User 1.0


Item {
    id: contactRec
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: searchfield.bottom
    anchors.bottom: parent.bottom
    visible: false

    Rectangle{
        id:manageAddressRec
        anchors.top: parent.top
        anchors.topMargin: 12
        anchors.left: parent.left
        anchors.leftMargin: 14
        anchors.right: parent.right
        anchors.rightMargin: 14
        height: 40
        color:"white"
        radius:6
        Image {
            id: manageAddressIcon
            source: "qrc:/MyChat/Icon/ManageAddress.png"
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.verticalCenter: parent.verticalCenter
            height: 15
            width: 19
        }
        Text {
            id: manageAddressTxt
            text: "通讯录管理"
            anchors.left: manageAddressIcon.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 14
            font.weight: 200
            color:"black"
        }
    }

    Rectangle {
        id: newFriendRec
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: manageAddressRec.bottom
        anchors.topMargin: 7
        height: 25
        color:"transparent"

        property bool isShow: false

        Image {
            id: newFriendArrow
            source: newFriendRec.isShow?"qrc:/MyChat/Icon/DownArrow.png":"qrc:/MyChat/Icon/RightArrow.png"
            width: 13
            height: width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            id: newFriendTxt
            text: "新的朋友"
            font.pixelSize: 14
            font.weight: 200
            color:"black"
            anchors.left: newFriendArrow.right
            anchors.leftMargin: 6
            anchors.verticalCenter: parent.verticalCenter
        }

        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                newFriendRec.isShow=!newFriendRec.isShow
                newFriendList.isShow=!newFriendList.isShow
            }
        }
    }

    ListView{
        id:newFriendList
        anchors.top: newFriendRec.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        visible: isShow?true:false
        height: isShow ? contentHeight : 0

        property int selectIndex: -1
        property bool isShow: false

        model: UserMgr.applyList
        onModelChanged: console.log("model changed")

        delegate: Rectangle{
            id:listRec
            anchors.left: parent.left
            anchors.right: parent.right
            height: 70
            color:newFriendList.selectIndex===index?"#D6D6D8":"transparent"
            Image {
                id: imgRec
                source: modelData.img
                width: 31
                height: width
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 38
                layer.enabled: true
                layer.smooth: true
                layer.effect: OpacityMask{
                    maskSource: Rectangle{
                        radius: 4
                        width: imgRec.width
                        height: imgRec.height
                        color:"white"
                    }
                }
            }
            //新消息红点
            Rectangle{
                id:newFriendMsgRec
                anchors.right: imgRec.right
                anchors.rightMargin: -5
                anchors.top: imgRec.top
                anchors.topMargin: -4
                width: 11
                height: width
                radius: width/2
                color: "#FA5151"
                visible: modelData.msg===0?true:false        //此model中msg为是否显示红点
            }
            Text {
                id: nameTxt
                text: modelData.name
                anchors.left: imgRec.right
                anchors.leftMargin: 8
                anchors.top: imgRec.top
                anchors.topMargin: -3
                font.pixelSize: 14
                font.weight: 400
                color: "black"
            }
            Text {
                id: msgTxt
                text: "我是："+modelData.name
                anchors.top: nameTxt.bottom
                anchors.topMargin: 5
                anchors.left: nameTxt.left
                font.pixelSize: 12
                font.weight: 200
                color: "#8F8F95"
            }

            Text {
                id: verifyStateTxt
                text: modelData.msg===0?"等待验证":"已通过"
                anchors.verticalCenter: nameTxt.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                font.pixelSize: 9
                font.weight: 200
                color: "#8F8F95"
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(newFriendList.currentIndex!==newFriendList.selectIndex){
                        listRec.color="#E2E2E4"
                    }
                    else{
                        listRec.color="#CBCBCD"
                    }
                }
                onExited: {
                    if(newFriendList.currentIndex!==newFriendList.selectIndex){
                        listRec.color="transparent"
                    }
                    else{
                        listRec.color="#DEDEDE"
                    }
                }
                onClicked: {
                    if(newFriendList.currentIndex!==newFriendList.selectIndex){
                        newFriendList.selectIndex=newFriendList.currentIndex
                        AppState.showFriendApply(modelData.uid,modelData.name,modelData.sex,modelData.img,modelData.msg)
                    }
                    else{
                        newFriendList.selectIndex=-1
                        AppState.showHome()
                    }
                    //取消红点，目前还没做信号
                }
            }
        }
    }

    //----------------联系人--------------
    Rectangle {
        id: friendRec
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: newFriendList.bottom
        anchors.topMargin: 7
        height: 25
        color:"transparent"

        property bool isShow: false

        Image {
            id: friendArrow
            source: friendRec.isShow?"qrc:/MyChat/Icon/DownArrow.png":"qrc:/MyChat/Icon/RightArrow.png"
            width: 13
            height: width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            id: friendTxt
            text: "联系人"
            font.pixelSize: 14
            font.weight: 200
            color:"black"
            anchors.left: friendArrow.right
            anchors.leftMargin: 6
            anchors.verticalCenter: parent.verticalCenter
        }

        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                friendRec.isShow=!friendRec.isShow
                friendList.isShow=!friendList.isShow
            }
        }
    }

    ListView{
        id:friendList
        anchors.top: friendRec.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        visible: isShow?true:false
        onCountChanged: console.log("friendList count", count)
        height: 100

        property int selectIndex: -1
        property bool isShow: false

        model: UserMgr.friendList

        delegate: Rectangle{
            id:friendListRec
            anchors.left: parent.left
            anchors.right: parent.right
            height: 55
            color:friendList.selectIndex===index?"#D6D6D8":"transparent"
            Image {
                id: friendImgRec
                source: modelData.img
                width: 31
                height: width
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 38
                layer.enabled: true
                layer.smooth: true
                layer.effect: OpacityMask{
                    maskSource: Rectangle{
                        radius: 4
                        width: friendImgRec.width
                        height: friendImgRec.height
                        color:"white"
                    }
                }
            }
            Text {
                id: friendNameTxt
                text: modelData.name
                anchors.left: friendImgRec.right
                anchors.leftMargin: 10
                anchors.verticalCenter: friendImgRec.verticalCenter
                font.pixelSize: 15
                font.weight: 400
                color: "black"
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(index!==friendList.selectIndex){
                        friendListRec.color="#E2E2E4"
                    }
                    else{
                        friendListRec.color="#CBCBCD"
                    }
                }
                onExited: {
                    if(index!==friendList.selectIndex){
                        friendListRec.color="transparent"
                    }
                    else{
                        friendListRec.color="#DEDEDE"
                    }
                }
                onClicked: {
                    if(index!==friendList.selectIndex){
                        friendList.selectIndex=index
                        AppState.showFriend(modelData.uid,modelData.name,modelData.sex,modelData.img)
                    }
                    else{
                        friendList.selectIndex=-1
                        AppState.showHome()
                    }
                }
            }
        }

        Connections{
            target: TcpMgr
            function onNotifyAuthFriend(fromuid,touid,name,sex,icon){
                newFriendList.verifyStateTxtW="已通过"
            }
        }
    }
}
