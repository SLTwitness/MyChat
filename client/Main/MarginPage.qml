import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0

Rectangle {
    id:marginPage
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 62
    color: "#F3E6E6E6"
    radius: 6
    border.width: 0.5
    border.color: "#D5D5D5"

    property int curIndex_mask: 1       //管理边栏状态

    //margin尖角补丁
    Rectangle{
        width: 5
        height: width
        anchors.right: marginPage.right
        anchors.top: marginPage.top
        color: marginPage.color
    }
    Rectangle{
        width: 5
        height: width
        anchors.right: marginPage.right
        anchors.bottom: marginPage.bottom
        color: marginPage.color
    }
    //margin补丁结束


    Image {
        id: userImg
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:parent.top
        anchors.topMargin: 21
        width: 36
        height: width
        source: "qrc:/MyChat/Icon/userImg_test.jpg"
        fillMode: Image.PreserveAspectCrop
        layer.enabled: true
        layer.smooth: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: userImg.width
                height: userImg.height
                radius: 6
                color: "white"
            }
        }
    }

    Rectangle{
        id:chatRec
        anchors.horizontalCenter:userImg.horizontalCenter
        anchors.top: userImg.bottom
        anchors.topMargin: 18
        width: 39
        height: width
        color:"transparent"
        radius: 6
        Image {
            id: chatIcon
            source: "qrc:/MyChat/Icon/Chat.png"
            anchors.centerIn: parent
            width: 20.5
            height: 19
        }
        ColorOverlay {
            id:chatIcon_mask
            anchors.fill: chatIcon
            source: chatIcon
            color:marginPage.curIndex_mask===1?"#07B75B":"#484848"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            z:1
            onEntered: {
                parent.color="#CECECE"
            }
            onExited: {
                parent.color="transparent"
            }
            onClicked: {
                if(marginPage.curIndex_mask!==1){
                    marginPage.curIndex_mask=1
                    AppState.showChatPage()
                }
            }
        }
    }

    Rectangle{
        id:contactRec
        anchors.horizontalCenter:userImg.horizontalCenter
        anchors.top: chatRec.bottom
        anchors.topMargin: 10
        width: 39
        height: width
        color:"transparent"
        radius: 6
        Image {
            id: contactIcon
            source: "qrc:/MyChat/Icon/Contact.png"
            anchors.centerIn: parent
            width: 21.5
            height: 17.5
        }
        ColorOverlay {
            id:contactIcon_mask
            anchors.fill: contactIcon
            source: contactIcon
            color:marginPage.curIndex_mask===2?"#07B75B":"#484848"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            z:1
            onEntered: {
                parent.color="#CECECE"
            }
            onExited: {
                parent.color="transparent"
            }
            onClicked: {
                if(marginPage.curIndex_mask!==2){
                    marginPage.curIndex_mask=2
                    AppState.showPage()

                }
                else{
                    AppState.showHome()
                }
            }
        }
    }

    Rectangle{
        id:collectRec
        anchors.horizontalCenter:userImg.horizontalCenter
        anchors.top: contactRec.bottom
        anchors.topMargin: 10
        width: 39
        height: width
        color:"transparent"
        radius: 6
        Image {
            id: collectIcon
            source: "qrc:/MyChat/Icon/Collect.png"
            anchors.centerIn: parent
            width: 18.5
            height: 20.5
        }
        ColorOverlay {
            id:collectIcon_mask
            anchors.fill: collectIcon
            source: collectIcon
            color:marginPage.curIndex_mask===3?"#07B75B":"#484848"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            z:1
            onEntered: {
                parent.color="#CECECE"
            }
            onExited: {
                parent.color="transparent"
            }
            onClicked: {
                if(marginPage.curIndex_mask!==3){
                    marginPage.curIndex_mask=3
                    AppState.showPage()
                }
                else{
                    AppState.showHome()
                }
            }
        }
    }


    Rectangle{
        id:configRec
        anchors.horizontalCenter:userImg.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 19
        width: 39
        height: width
        color:"transparent"
        radius: 6
        Image {
            id: configIcon
            source: "qrc:/MyChat/Icon/Config.png"
            anchors.centerIn: parent
            width: 18
            height: 12
        }
        ColorOverlay{
            id:configIcon_mask
            anchors.fill: configIcon
            source: configIcon
            color:"#484848"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            z:1
            onEntered: {
                parent.color="#CECECE"
            }
            onExited: {
                parent.color="transparent"
            }
            onClicked: {

            }
        }
    }
}
