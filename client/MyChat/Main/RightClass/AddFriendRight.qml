import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0
import QtQuick.Controls.Material
import User 1.0
import Tcp 1.0


Item{
    id:newFriendRec
    anchors.fill: parent
    visible: false

    property string toName: ""
    property int toSex: 1
    property string toImg: ""
    property int toUid: -1

    property bool applyAgreeTxtVisible: applyAgreeTxt.visible
    property bool agreeApplyRecVisible: agreeApplyRec.visible

    Image {
        id: newFriendImg
        source: newFriendRec.toImg
        anchors.bottom: newFriendBrief.top
        anchors.bottomMargin: 18
        anchors.left: newFriendBrief.left
        width: 60
        height: width
        layer.enabled: true
        layer.smooth: true
        layer.effect: OpacityMask{
            maskSource: Rectangle{
                radius: 4
                width: newFriendImg.width
                height: newFriendImg.height
                color:"white"
            }
        }
    }
    Text {
        id: newFriendName
        text: newFriendRec.toName===""?"test":newFriendRec.toName
        font.pixelSize: 14
        font.weight: 400
        color:"black"
        anchors.left: newFriendImg.right
        anchors.leftMargin: 17
        anchors.top: newFriendImg.top
    }
    Image {
        id: newFriendSex
        source: newFriendRec.toSex===1?"qrc:/MyChat/Icon/ManIcon.png":"qrc:/MyChat/Icon/WomanIcon.png"
        anchors.bottom: newFriendName.bottom
        anchors.bottomMargin: 2.5
        anchors.left: newFriendName.right
        anchors.leftMargin: 4
        width: 12.5
        height: width
    }

    Rectangle{
        id:newFriendBrief
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 185
        width: 320
        height: 68
        color:"#D6D6D8"
        radius:5
        Text {
            id: myBriefTxt
            text: "我：我是"+UserMgr.getName()
            font.pixelSize: 12
            font.weight: 200
            color:"#9F9FA6"
            anchors.left: parent.left
            anchors.leftMargin: 7
            anchors.top: parent.top
            anchors.topMargin: 8
        }
        Text{
            id:friendBriefTxt
            text: newFriendRec.toName+"：我是"+newFriendRec.toName
            font.pixelSize: 12
            font.weight: 200
            color:"#9F9FA6"
            anchors.left: myBriefTxt.left
            anchors.top: myBriefTxt.bottom
            anchors.topMargin: 1
        }
    }

    //分割线1
    Rectangle{
        id:dividRec1
        height: 0.7
        width: newFriendBrief.width
        color:"#D5D5D5"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: newFriendBrief.bottom
        anchors.topMargin: 18
    }

    Text {
        id: fromWhereTxt
        text: "来源"
        font.pixelSize: 14
        font.weight: 200
        color:"#9F9FA6"
        anchors.left: newFriendBrief.left
        anchors.top: dividRec1.bottom
        anchors.topMargin: 17
    }
    Text {
        id: sourceTxt
        text: "朋友验证"
        font.pixelSize: 14
        font.weight: 200
        color:"black"
        anchors.left: fromWhereTxt.right
        anchors.leftMargin: 50
        anchors.bottom: fromWhereTxt.bottom
    }

    //分割线2
    Rectangle{
        id:dividRec2
        height: 0.7
        width: newFriendBrief.width
        color:"#D5D5D5"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: sourceTxt.bottom
        anchors.topMargin: 17
    }

    Rectangle{
        id:agreeApplyRec
        height: 30
        width: 105
        color:"#D6D6D8"
        radius: 5
        anchors.top: dividRec2.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        visible: agreeApplyRecVisible
        Text {
            id: agreeApplyTxt
            text: "通过验证"
            font.pixelSize: 14
            font.weight: 200
            color:"black"
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                agreeApplyRec.color="#D3D3D3"
            }
            onExited: {
                agreeApplyRec.color="#D6D6D8"
            }
            onClicked: {
                //通过验证
                TcpMgr.authFriendApply(UserMgr.getUid(),newFriendRec.toUid,"")
                //通过后接收方直接添加好友
                UserMgr.addFriend(newFriendRec.toUid,newFriendRec.toName,newFriendRec.toSex,newFriendRec.toImg)

                //首次时，手动设为已通过
                applyAgreeTxt.visible=true
                agreeApplyRec.visible=false
            }
        }
    }
    Text{
        id:applyAgreeTxt
        anchors.centerIn: agreeApplyRec
        font.pixelSize: 14
        font.weight: 200
        color:"#9F9FA6"
        text:"已通过"
        visible: newFriendRec.applyAgreeTxtVisible
    }
}
