import QtQuick 2.15
import QtQuick.Controls.Material
import Net 1.0
import MyChat 1.0                
import Tcp 1.0

Rectangle{
    id:toenterRec
    anchors.fill: parent
    visible: false
    color:"transparent"

    function clearFocus(){
        emailTxtFld.focus=false
        passwdTxtFld.focus=false
    }

    Text{
        id:emailTxt
        text:"邮箱 :"
        font.pixelSize: 14
        anchors.top: parent.top
        anchors.topMargin: 105
        anchors.left: parent.left
        anchors.leftMargin: 27
    }
    TextField{
        id:emailTxtFld
        anchors.left: emailTxt.right
        anchors.leftMargin: 10
        anchors.verticalCenter: emailTxt.verticalCenter
        width: 180
        height: 35
        Material.accent: "#07C160"
        font.pixelSize: 13
        leftPadding:12
    }

    Text{
        id:passwdTxt
        text:"密码 :"
        font.pixelSize: 14
        anchors.top: emailTxt.bottom
        anchors.topMargin: 60
        anchors.left: emailTxt.left
    }
    TextField{
        id:passwdTxtFld
        anchors.left: passwdTxt.right
        anchors.leftMargin: 10
        anchors.verticalCenter: passwdTxt.verticalCenter
        width: 180
        height: 35
        Material.accent: "#07C160"
        font.pixelSize: 13
        leftPadding:12
    }

    Rectangle{
        id:tologinRec
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: passwdTxt.bottom
        anchors.topMargin: 60
        color:"#07C160"
        radius: 4
        width: 180
        height: 36

        Text {
            id: tologinText
            text: "登入"
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
                tologinRec.color="#06B75B"
            }
            onExited: {
                cursorShape=Qt.ArrowCursor
                tologinRec.color="#07C160"
            }
            onClicked: {
                NetBridge.loginRequst(emailTxtFld.text,passwdTxtFld.text)
            }
        }

        Timer{
            id:loadPage
            interval: 2000
            repeat: false
            onTriggered: {
                AppState.loginSuccess()
            }
        }
    }
    Text{
        id:login_hint
        anchors.horizontalCenter: tologinRec.horizontalCenter
        anchors.top: tologinRec.bottom
        anchors.topMargin: 5
        text:""
        font.pixelSize: 12
        color:"black"
        Connections{
            target: NetBridge
            function onLoginSend(success,message,host,port,uid,token){
                if(success){
                    login_hint.color="green"

                    //建立tcp连接
                    TcpMgr.connectToChatServer(host,port,uid,token)

                    loadPage.start()
                }
                else{
                    login_hint.color="red"
                }
                login_hint.text=message
            }
        }
    }


    Text{
        id:registerTxt
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 27
        anchors.right: divid1.left
        anchors.rightMargin: 10
        text:"注册"
        color:"#576B95"
        font.pixelSize: 14
        font.weight: 400

        MouseArea{
            anchors.fill: parent
            onClicked: {
                verifyRec.visible=true
                toenterRec.visible=false
            }
        }
    }

    Rectangle{                                                          //分割线
        id:divid1
        width: 1
        height: 20
        color:"#EBEBEB"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: registerTxt.verticalCenter
        radius: 3
    }

    Text{
        id:leaveTxt
        anchors.verticalCenter: registerTxt.verticalCenter
        anchors.left: divid1.right
        anchors.leftMargin: registerTxt.anchors.rightMargin
        text:"离开"
        color:"#576B95"
        font.pixelSize: 14
        font.weight: 400

        MouseArea{
            anchors.fill: parent
            onClicked: {
                loginRec.visible=true
                toenterRec.visible=false
            }
        }
    }
}
