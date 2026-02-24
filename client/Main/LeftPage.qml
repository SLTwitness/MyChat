import QtQuick 2.15
import QtQuick.Controls.Material
import MyChat 1.0
import Qt5Compat.GraphicalEffects

Rectangle{
    id:leftPage
    anchors.left: marginPage.right
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 210
    color: "#F7F7F7"
    border.width: 0.5
    border.color: "#D5D5D5"


    TextField{
        id:searchfield
        anchors.left: parent.left
        anchors.leftMargin: 14
        anchors.top: parent.top
        anchors.topMargin: 28
        width: 147
        height: 27
        placeholderText: "搜索"
        color:"#A6A6A6"
        font.pixelSize: 12
        font.weight: 200
        leftPadding: 30
        bottomPadding: 6
        Material.accent: "#07C160"

        background: Rectangle{
            color:"#EAEAEA"
            radius:3
        }

        Image {
            id: searchIcon
            source: "qrc:/MyChat/Icon/Search.png"
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: 12
            height: width
        }

        Connections{
            target: AppState
            function onSearchFocus(){
                searchfield.focus=false
            }
        }
    }

    Rectangle{
        id:quickOptRec
        anchors.verticalCenter: searchfield.verticalCenter
        anchors.left: searchfield.right
        anchors.leftMargin:8
        height: searchfield.height
        width: height
        color:"#EAEAEA"
        radius: 3

        Image {
            id: quickOptIcon
            source: "qrc:/MyChat/Icon/Add.png"
            anchors.centerIn: parent
            width: 12
            height: width
        }
        ColorOverlay{
            id: quickOptIcon_mask
            source:quickOptIcon
            anchors.fill: quickOptIcon
            color:"#A6A6A6"
        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                parent.color="#E4E4E4"
            }
            onExited: {
                parent.color="#EAEAEA"
            }
        }
    }


    /*----------------------------------消息列表----------------------------------*/
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
                        AppState.showChat(name)
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
                AppState.showChat(model.get(0).name)
            }
        }
    }
    /*----------------------------------消息列表----------------------------------*/


    Connections{
        target: AppState
        function onShowPage(){
            chatList.visible=false
            searchfield.visible=false
            quickOptRec.visible=false
        }
        function onShowChatPage(){
            chatList.selectIndex=-1
            chatList.visible=true
            searchfield.visible=true
            quickOptRec.visible=true
        }
    }
}
