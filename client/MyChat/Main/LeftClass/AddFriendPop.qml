import QtQuick 2.15
import QtQuick.Controls.Material
import MyChat 1.0
import Qt5Compat.GraphicalEffects
import Tcp 1.0
import User 1.0


Popup{
    id:addFriendPop
    x:(mainWindow.x-width+100)/2
    y:(mainWindow.y-height+400)/2
    width: 360
    height: 660
    parent: Overlay.overlay

    property string sendAddFriendTxtAreaTxt: sendAddFriendTxtArea.text
    property string addFriendNoteTxtFldTxt: addFriendNoteTxtFld.text

    //拖动区域
    MouseArea {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40

        cursorShape: Qt.PointingHandCursor
        property point pressPos

        onPressed: (mouse) => {
            pressPos = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged: (mouse) => {
            if (pressed) {
                addFriendPop.x += mouse.x - pressPos.x
                addFriendPop.y += mouse.y - pressPos.y
            }
        }
    }

    background: Rectangle{
        id:addFriendRec
        color:"#EEEEF0"
        radius: 6
        border.color: "#BBBBBB"
        border.width: 1

        layer.enabled: true
        layer.effect: DropShadow{
            radius: 16
            //samples: 20
            color: "#33000000"
            verticalOffset: 2
            horizontalOffset: 0
        }
    }

    Item{
        anchors.fill: parent
        Text {
            id: addFriendRecTxt
            text: "申请添加朋友"
            font.weight: 200
            font.pixelSize: 14
            color:"black"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 5
        }
        Text {
            id: sendAddFriendTxt
            text: "发送添加朋友申请"
            font.weight: 200
            font.pixelSize: 12
            color:"#9F9FA6"
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.top: addFriendRecTxt.bottom
            anchors.topMargin: 5
        }
        TextArea{
            id:sendAddFriendTxtArea
            text:addFriendPop.sendAddFriendTxtAreaTxt
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: sendAddFriendTxt.bottom
            anchors.topMargin: 9
            width: 310
            height: 80
            font.pixelSize: 14
            font.weight: 200
            topPadding: 15
            Material.accent: "#07C160"
            background: Rectangle{
                color:"white"
                radius: 8
                border.width: 0.8
                border.color: "#DCDCDD"
            }
        }

        Text {
            id: addFriendNoteTxt
            text: "备注"
            anchors.left: sendAddFriendTxt.left
            anchors.top: sendAddFriendTxtArea.bottom
            anchors.topMargin: 20
            font.weight: 200
            font.pixelSize: 12
            color:"#9F9FA6"
        }
        TextField{
            id:addFriendNoteTxtFld
            anchors.top: addFriendNoteTxt.bottom
            anchors.topMargin: 9
            anchors.horizontalCenter: parent.horizontalCenter
            //placeholderText: "test"
            text:addFriendPop.addFriendNoteTxtFldTxt
            width: sendAddFriendTxtArea.width
            height: 47
            font.pixelSize: 14
            Material.accent: "#07C160"
            background: Rectangle{
                color:"white"
                radius: 8
                border.width: 0.8
                border.color: "#DCDCDD"
            }
        }

        Text {
            id: addFriendLabelTxt
            text: "标签"
            anchors.left: sendAddFriendTxt.left
            anchors.top: addFriendNoteTxtFld.bottom
            anchors.topMargin: 20
            font.weight: 200
            font.pixelSize: 12
            color:"#9F9FA6"
        }
        TextField{
            id:addFriendLabelTxtFld
            anchors.top: addFriendLabelTxt.bottom
            anchors.topMargin: 9
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: "搜索或创建标签..."
            width: sendAddFriendTxtArea.width
            height: 47
            font.pixelSize: 14
            Material.accent: "#07C160"
            background: Rectangle{
                color:"white"
                radius: 8
                border.width: 0.8
                border.color: "#DCDCDD"
            }
        }

        Text {
            id: addFriendLimitTxt
            text: "朋友权限"
            anchors.left: sendAddFriendTxt.left
            anchors.top: addFriendLabelTxtFld.bottom
            anchors.topMargin: 20
            font.weight: 200
            font.pixelSize: 12
            color:"#9F9FA6"
        }
        Rectangle{
            id:addFriendLimitTxtFld
            anchors.top: addFriendLimitTxt.bottom
            anchors.topMargin: 7
            anchors.horizontalCenter: parent.horizontalCenter
            color:"white"
            radius: 8
            border.width: 0.8
            border.color: "#DCDCDD"
            width: sendAddFriendTxtArea.width
            height: 87
            Text {
                id: allAcceptTxt
                text: "聊天、朋友圈、MyChat运动等"
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.bottom: addFriendDivid1.top
                anchors.bottomMargin: 12
                font.pixelSize: 14
                font.weight: 200
            }
            //分割线
            Rectangle{
                id:addFriendDivid1
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: sendAddFriendTxtArea.width-40
                height: 0.8
                color:"#F0F0F0"
            }
            Text {
                id: allLimitTxt
                text: "仅聊天"
                anchors.left: allAcceptTxt.left
                anchors.top: addFriendDivid1.bottom
                anchors.topMargin: 12
                font.pixelSize: 14
                font.weight: 200
            }
        }

        Text {
            id: addFriendCircleTxt
            text: "朋友圈和状态"
            anchors.left: sendAddFriendTxt.left
            anchors.top: addFriendLimitTxtFld.bottom
            anchors.topMargin: 20
            font.weight: 200
            font.pixelSize: 12
            color:"#9F9FA6"
        }
        Rectangle{
            id:addFriendCircleTxtFld
            anchors.top: addFriendCircleTxt.bottom
            anchors.topMargin: 7
            anchors.horizontalCenter: parent.horizontalCenter
            color:"white"
            radius: 8
            border.width: 0.8
            border.color: "#DCDCDD"
            width: sendAddFriendTxtArea.width
            height: addFriendLimitTxtFld.height
            Text {
                id: notSeeTaTxt
                text: "不让他（她）看"
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.bottom: addFriendDivid2.top
                anchors.bottomMargin: 12
                font.pixelSize: 14
                font.weight: 200
            }
            //分割线
            Rectangle{
                id:addFriendDivid2
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: sendAddFriendTxtArea.width-40
                height: 0.8
                color:"#F0F0F0"
            }
            Text {
                id: notSeeMeTxt
                text: "不看他（她）"
                anchors.left: notSeeTaTxt.left
                anchors.top: addFriendDivid2.bottom
                anchors.topMargin: 12
                font.pixelSize: 14
                font.weight: 200
            }
        }

        // "确定" / "取消" 按钮
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 65
            anchors.top: addFriendCircleTxtFld.bottom
            anchors.topMargin: 18
            width: 95
            height: 30
            color:"#00C375"
            radius: 6
            Text {
                text: "确定"
                font.weight: 400
                font.pixelSize: 14
                color:"white"
                anchors.centerIn: parent
            }
            MouseArea{
                hoverEnabled: true
                anchors.fill: parent
                onEntered: {
                    parent.color="#00B96F"
                }
                onExited: {
                    parent.color="#00C375"
                }
                onClicked: {
                    TcpMgr.addFriendSend(UserMgr.getUid(),AppState.searchUserUid,UserMgr.getName())
                    addFriendPop.close()
                    searchUserPop.close()
                }
            }
        }
        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 65
            anchors.top: addFriendCircleTxtFld.bottom
            anchors.topMargin: 18
            width: 95
            height: 30
            color:"#E2E2E4"
            radius: 6
            Text {
                text: "取消"
                font.weight: 200
                font.pixelSize: 14
                color:"black"
                anchors.centerIn: parent
            }
            MouseArea{
                hoverEnabled: true
                anchors.fill: parent
                onEntered: {
                    parent.color="#DBDBDD"
                }
                onExited: {
                    parent.color="#E2E2E4"
                }
                onClicked: {
                    addFriendPop.close()
                }
            }
        }
    }
}
