import QtQuick 2.15
import QtQuick.Controls.Material
import MyChat 1.0
import Qt5Compat.GraphicalEffects
import Tcp 1.0
import User 1.0

Rectangle{
    id:leftPage
    anchors.left: marginPage.right
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 210
    color: "#F7F7F7"
    border.width: 0.5
    border.color: "#D5D5D5"


    //-------------------------------搜索框------------------------------
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

        onTextChanged: {
            if(searchfield.text!==""){
                searchPop.open()
            }
            else searchPop.close()
        }

        Connections{
            target: AppState
            function onSearchFocus(){
                searchfield.focus=false
            }
        }
    }

    Popup{
        id:searchPop
        x:searchfield.x
        y:searchfield.y+searchfield.height+3
        width: 310
        height: 60
        background: Rectangle{
            color:"white"
            radius: 5
            border.color: "white"
            border.width: 0.5
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="#EDEDED"
                }
                onExited: {
                    parent.color="white"
                }
                onClicked: {
                    searchUserPop.open()
                    TcpMgr.searchUserSend(searchfield.text)
                }
            }
        }
        Item{
            anchors.fill: parent
            Image {
                id: searchUserIcon
                source: "qrc:/MyChat/Icon/SearchUser.png"
                anchors.left: parent.left
                anchors.leftMargin: -10
                anchors.top: parent.top
                anchors.topMargin: -10
                scale: 0.7
            }
            Text {
                id: searchUserTxt
                text: "网络查找手机号/QQ号："
                color: "black"
                font.pixelSize: 14
                font.weight: 400
                anchors.left: searchUserIcon.right
                anchors.verticalCenter: searchUserIcon.verticalCenter
            }
            Text {
                id: searchUidTxt
                text: searchfield.text
                color:"#00C375"
                elide: Text.ElideRight
                width: 90
                font.pixelSize: 14
                font.weight: 400
                anchors.left: searchUserTxt.right
                anchors.verticalCenter: searchUserTxt.verticalCenter
            }
        }
    }

    Popup{
        id:searchUserPop
        x:(mainWindow.x-width)/2
        y:(mainWindow.y-height+200)/2
        width: 330
        height: 455
        //popupType: Popup.Window
        closePolicy: Popup.CloseOnEscape

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
                    searchUserPop.x += mouse.x - pressPos.x
                    searchUserPop.y += mouse.y - pressPos.y
                }
            }
        }

        background: Rectangle{
            id:searchUserRec
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
                id:addFriendtxt
                text: "添加朋友"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 0
                font.pixelSize: 16
                font.weight: 400
            }

            Rectangle{
                id:searchUserClose
                anchors.right: parent.right
                anchors.rightMargin: -11
                anchors.top: parent.top
                anchors.topMargin: -11
                width: 46
                height: 30
                color:"#EEEEF0"
                radius: 6
                //圆角补丁
                Rectangle{
                    anchors.top: searchUserClose.top
                    anchors.left: searchUserClose.left
                    width: 10
                    height: width
                    color:searchUserClose.color
                }
                Rectangle{
                    anchors.bottom: searchUserClose.bottom
                    anchors.left: searchUserClose.left
                    width: 10
                    height: width
                    color:searchUserClose.color
                }
                Rectangle{
                    anchors.bottom: searchUserClose.bottom
                    anchors.right: searchUserClose.right
                    width: 10
                    height: width
                    color:searchUserClose.color
                }
                //补丁结束
                Image {
                    id: loginClose
                    anchors.centerIn: parent
                    source: "qrc:/MyChat/Icon/LogClose.png"
                    width: 12
                    height: width
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        searchUserClose.color="#ED4C4C"
                    }
                    onExited: {
                        searchUserClose.color="#EEEEF0"
                    }
                    onClicked: {
                        searchUserPop.close()
                    }
                }
            }

            TextField{
                id:searchfiedPop
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.top: addFriendtxt.bottom
                anchors.topMargin: 20
                width: 200
                height: 40
                text: searchfield.text
                font.pixelSize: 14
                font.weight: 400
                leftPadding: 35
                bottomPadding: 8
                Material.accent: "#07C160"
                background: Rectangle{
                    color:"#E2E2E4"
                    radius:5
                }
                Image {
                    id: searchIconPop
                    source: "qrc:/MyChat/Icon/Search.png"
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    width: 15
                    height: width
                }
            }

            Rectangle{
                id:searchRecPop
                anchors.left: searchfiedPop.right
                anchors.leftMargin: 10
                anchors.verticalCenter: searchfiedPop.verticalCenter
                height: searchfiedPop.height
                width: 60
                color:"#00C375"
                radius: 4
                Text{
                    anchors.centerIn: parent
                    text:"搜索"
                    color:"white"
                    font.pixelSize: 14
                }
            }

            Rectangle{
                id:searchUserResRec
                width: searchfiedPop.width+searchRecPop.anchors.leftMargin+searchRecPop.width
                height: 240
                color:"white"
                anchors.top: searchfiedPop.bottom
                anchors.topMargin: 15
                anchors.left: searchfiedPop.left
                radius: 6
                visible: true
                Image {
                    id:searchUserImg
                    source: AppState.searchUserIcon
                    anchors.left: parent.left
                    anchors.leftMargin: 25
                    anchors.top: parent.top
                    anchors.topMargin: 28
                    width: 60
                    height: width
                    fillMode: Image.PreserveAspectCrop
                    layer.enabled: true
                    layer.smooth: true
                    layer.effect: OpacityMask {
                        maskSource: Rectangle {
                            width: searchUserImg.width
                            height: searchUserImg.height
                            radius: 6
                            color: "white"
                        }
                    }
                }
                Text {
                    id: searchUserName
                    text: "test"
                    anchors.left: searchUserImg.right
                    anchors.leftMargin: 17
                    anchors.top: searchUserImg.top
                    font.pixelSize: 16
                }
                Image {
                    id: searchUserSex
                    source: sex===1?"qrc:/MyChat/Icon/ManIcon.png":"qrc:/MyChat/Icon/WomanIcon.png"
                    anchors.left: searchUserName.right
                    anchors.leftMargin: 8
                    anchors.verticalCenter: searchUserName.verticalCenter
                    height: 15
                    width: height
                    property int sex: 1
                }

                //分割线
                Rectangle{
                    id:searchUserDivid
                    anchors.top:searchUserImg.bottom
                    anchors.topMargin: 10
                    anchors.left: searchUserImg.left
                    width: parent.width-2*searchUserImg.anchors.leftMargin
                    height: 0.8
                    color:"#F0F0F0"
                }

                Rectangle{
                    id:addFriendButton
                    anchors.top: searchUserDivid.bottom
                    anchors.topMargin: 25
                    anchors.horizontalCenter: searchUserDivid.horizontalCenter
                    width: 110
                    height: 30
                    radius: 6
                    color:"#EDEDED"
                    Text {
                        text: "添加到通讯录"
                        font.pixelSize: 14
                        font.weight: 200
                        color:"black"
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            parent.color="#E6E6E6"
                        }
                        onExited: {
                            parent.color="#EDEDED"
                        }
                        onClicked: {
                            addFriendPop.open()
                            addFriendPop.sendAddFriendTxtAreaTxt="我是"+UserMgr.getName()
                            addFriendPop.addFriendNoteTxtFldTxt=searchUserName.text
                        }
                    }
                }

                Connections{
                    target: TcpMgr
                    function onSearchUserRes(success,uid,name,sex,icon){
                        if(success){
                            console.log("查找成功")
                            searchUserName.text=name
                            searchUserSex.sex=sex

                            //记录搜索用户的 uid,name,sex
                            AppState.searchUserUid=uid
                            AppState.searchUserSex=sex
                            AppState.searchUserName=name
                            AppState.searchUserIcon=icon

                            searchUserResRec.visible=true
                            searchUserErr.visible=false
                        }
                        else{
                            console.log("查找失败")
                            searchUserResRec.visible=false
                            searchUserErr.visible=true
                        }
                    }
                }
            }

            Text {
                id: searchUserErr
                text: "无法找到该用户，请检查你填写的账号是否正确"
                color:"#9F9FA6"
                font.pixelSize: 12
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 240
                visible: false
            }
        }
    }
    //-------------------------------搜索框------------------------------


    //----------------------------添加好友界面---------------------------
    AddFriendPop{
        id:addFriendPop
    }
    //----------------------------添加好友界面---------------------------


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
    ChatClass{
        id:chatList
    }
    /*----------------------------------消息列表----------------------------------*/


    /*----------------------------------通讯录---------------------------------*/
    ContactClass{
        id: contactRec
    }
    /*----------------------------------通讯录---------------------------------*/

    Connections{
        target: AppState
        function onShowPage(){
            chatList.visible=false
            //searchfield.visible=false
            //quickOptRec.visible=false
        }
        function onShowChatPage(){
            chatList.selectIndex=-1
            chatList.visible=true
            contactRec.visible=false
            //searchfield.visible=true
            //quickOptRec.visible=true
        }
        function onShowFriendPage(){
            chatList.visible=false
            contactRec.visible=true
        }
    }
}
