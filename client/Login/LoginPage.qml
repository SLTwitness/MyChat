import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import MyChat 1.0

Rectangle{
    id:backRec
    anchors.fill:parent
    color:"transparent"

    Rectangle{
        id:mainRec
        anchors.fill: parent
        color:"#FFFFFF"
        border.width: 0.5
        border.color: "#D5D5D5"
        radius: 8

        Text{                                       //top栏按钮区
            anchors.left: parent.left
            anchors.leftMargin: 13
            anchors.top: parent.top
            anchors.topMargin: 9
            text:"微信"
            color:"#9E9E9E"
            font.pixelSize:14
            font.weight: 400
        }

        Row{
            id:buttonRow
            anchors.right: parent.right
            anchors.rightMargin: mainRec.border.width
            anchors.top: parent.top
            anchors.topMargin: mainRec.border.width

            Rectangle{
                id:loginSettingRec
                width: 45
                height: 40
                color:"white"

                Image {
                    id: loginSetting
                    anchors.centerIn: parent
                    source: "qrc:/MyChat/Icon/LogConfig.png"
                    width: 15
                    height: 13
                }

                MouseArea{
                    z:1                                     //设置rec的鼠标区域
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        cursorShape=Qt.PointingHandCursor
                        loginSettingRec.color="#F2F2F2"
                    }
                    onExited: {
                        cursorShape=Qt.ArrowCursor
                        loginSettingRec.color="white"
                    }
                }
            }

            Rectangle{
                id:loginCloseRec
                width: loginSettingRec.width
                height: loginSettingRec.height
                color:"white"
                radius: mainRec.radius

                //打点补丁让x按钮的左上、左下、右下角变尖
                Rectangle{
                    anchors.top: loginCloseRec.top
                    anchors.left: loginCloseRec.left
                    width: 10
                    height: width
                    color:loginCloseRec.color
                }
                Rectangle{
                    anchors.bottom: loginCloseRec.bottom
                    anchors.left: loginCloseRec.left
                    width: 10
                    height: width
                    color:loginCloseRec.color
                }
                Rectangle{
                    anchors.bottom: loginCloseRec.bottom
                    anchors.right: loginCloseRec.right
                    width: 10
                    height: width
                    color:loginCloseRec.color
                }
                //补丁打完

                Image {
                    id: loginClose
                    anchors.centerIn: parent
                    source: "qrc:/MyChat/Icon/LogClose.png"
                    width: 12
                    height: width
                }

                MouseArea{
                    z:1                                     //设置rec的鼠标区域
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        cursorShape=Qt.PointingHandCursor
                        loginCloseRec.color="#ED4C4C"
                    }
                    onExited: {
                        cursorShape=Qt.ArrowCursor
                        loginCloseRec.color="white"
                    }
                    onClicked: {
                        Qt.quit();
                    }
                }
            }
        }
        /*-------------------top栏按钮区结束---------------*/


        Rectangle{
            id:loginRec
            anchors.fill: parent
            color:"transparent"
            visible: true

            Image {
                id: userImg
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top:parent.top
                anchors.topMargin: 60
                width: 82
                height: 82
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

            Text{
                id:userId
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: userImg.bottom
                anchors.topMargin: 30
                text:"用户名测试"
                font.pixelSize: 15
                color:"black"
            }

            Rectangle{
                id:enterRec
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 75
                color:"#07C160"
                radius: 4
                width: 180
                height: 36

                Text {
                    id: enterText
                    text: "进入微信"
                    anchors.centerIn: parent
                    color:"white"
                    font.pixelSize: 14
                    font.weight: 500
                }

                MouseArea{
                    z:1
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        cursorShape=Qt.PointingHandCursor
                        enterRec.color="#06B75B"
                    }
                    onExited: {
                        cursorShape=Qt.ArrowCursor
                        enterRec.color="#07C160"
                    }
                    onClicked: {
                        loginRec.visible=false
                        verifyRec.visible=true

                        /*测试用：发送验证信号*/
                        AppState.verifyEnter()
                        /*测试用：发送验证信号*/
                    }
                }
            }

            Text{
                id:changeAccount
                text:"切换账号"
                anchors.top: enterRec.bottom
                anchors.topMargin: 22
                anchors.left: parent.left
                anchors.leftMargin: 67
                color:"#576B95"
                font.pixelSize: 14
                font.weight: 400
            }

            Rectangle{                                                          //分割线
                id:divid1
                width: 1
                height: 20
                color:"#EBEBEB"
                anchors.verticalCenter: changeAccount.verticalCenter
                anchors.left: changeAccount.right
                anchors.leftMargin: 8
                radius: 3
            }

            Text{
                id:onlyTransferFile
                text:"仅传输文件"
                anchors.verticalCenter: changeAccount.verticalCenter
                anchors.left: divid1.right
                anchors.leftMargin: 8
                color:"#576B95"
                font.pixelSize: 14
                font.weight: 400
            }
        }


        MouseArea{
            z:0                                             //整个窗口的鼠标区域（这边放到顶部应该就行了，但记录一下方法吧）
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: (mouse)=>{
                if(mouse.button===Qt.LeftButton){
                    var buttonRowPos = buttonRow.mapToItem(this, 0, 0)          //将buttonRow的左上角(0,0)映射到此MouseArea中
                    var enterRecPos=enterRec.mapToItem(this,0,0)

                    if(mouse.x<buttonRowPos.x || mouse.x>buttonRowPos.x+buttonRow.width ||
                       mouse.y<buttonRowPos.y || mouse.y>buttonRowPos.y+buttonRow.height){
                        if(mouse.x<enterRecPos.x || mouse.x>enterRecPos.x+enterRec.width ||
                           mouse.y<enterRecPos.y || mouse.y>enterRecPos.y+enterRec.height){
                            mainWindow.startSystemMove()
                        }
                    }
                }
            }
        }
    }


    VerifyPage{
        id:verifyRec
        anchors.fill: parent
        visible: false
    }
}
