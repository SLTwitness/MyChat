import QtQuick 2.15
import QtQuick.Controls.Material
import MyChat 1.0
import Qt5Compat.GraphicalEffects
import Tcp 1.0
import User 1.0

ListView{
    id:chatList
    anchors.top: searchfield.bottom
    anchors.topMargin: 13
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right

    property int selectIndex: -1

    model: ListModel{
        ListElement{
            img:"qrc:/MyChat/Icon/userImg_test.jpg"
            name:"测试"
            message:"可以吗"
            time:"01:00"
        }
    }

    delegate: Rectangle{
        id:listRec
        anchors.left: parent.left
        anchors.right: parent.right
        height: 66
        color:chatList.selectIndex===index?"#DEDEDE":"transparent"
        Image {
            id: imgRec
            source: img
            width: 38
            height: width
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 13
            anchors.topMargin: (listRec.height-imgRec.height)/2
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
        Text {
            id: nameRec
            text: name
            anchors.left: imgRec.right
            anchors.top: imgRec.top
            anchors.leftMargin: 10
            anchors.topMargin: 0
            font.pixelSize: 14
            width: 100
            color:"#161616"
            elide: Text.ElideRight
        }
        Text {
            id: timeRec
            text: time
            color: "#9E9E9E"
            anchors.verticalCenter: nameRec.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 12
            font.pixelSize: 10
        }
        Text {
            id: messageRec
            text: message
            color: "#9E9E9E"
            anchors.left: nameRec.left
            anchors.bottom: imgRec.bottom
            anchors.bottomMargin: -3
            font.pixelSize: 12
            width: 125
            elide: Text.ElideRight
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                if(chatList.currentIndex!==chatList.selectIndex){
                    listRec.color="#EAEAEA"
                }
                else{
                    listRec.color="#D3D3D3"
                }
            }
            onExited: {
                if(chatList.currentIndex!==chatList.selectIndex){
                    listRec.color="transparent"
                }
                else{
                    listRec.color="#DEDEDE"
                }
            }
            onClicked: {
                if(chatList.currentIndex!==chatList.selectIndex){
                    chatList.selectIndex=chatList.currentIndex
                    AppState.showChat(1,name,"qrc:/MyChat/Icon/userImg_test.jpg")
                }
                else{
                    chatList.selectIndex=-1
                    AppState.showHome()
                }
            }
        }
    }

    Component.onCompleted: {
        if(count>0){
            currentIndex=0
            selectIndex=0
            AppState.showChat(1,model.get(0).name,"qrc:/MyChat/Icon/userImg_test.jpg")
        }
    }
}
