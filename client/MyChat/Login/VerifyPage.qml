import QtQuick 2.15
import QtQuick.Controls.Material
import Net 1.0
import MyChat 1.0

Rectangle{
    id:verifyRec
    anchors.fill: parent
    visible: false
    color:"transparent"

    function clearFocus(){
        userTxtFld.focus=false
        emailTxtFld.focus=false
        passwdTxtFld.focus=false
        verifyCodeFld.focus=false
    }

    Text{
        id:userTxt
        text:"昵称 :"
        font.pixelSize: 14
        anchors.top: parent.top
        anchors.topMargin: 70
        anchors.left: parent.left
        anchors.leftMargin: 27
    }
    TextField{
        id:userTxtFld
        anchors.left: userTxt.right
        anchors.leftMargin: 10
        anchors.verticalCenter: userTxt.verticalCenter
        width: 180
        height: 35
        Material.accent: "#07C160"
        font.pixelSize: 13
        leftPadding:12
    }

    Text{
        id:emailTxt
        text:"邮箱 :"
        font.pixelSize: 14
        anchors.top: userTxt.bottom
        anchors.topMargin: 35
        anchors.left: userTxt.left
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
        anchors.topMargin: emailTxt.anchors.topMargin
        anchors.left: userTxt.left
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

    Text{
        id:verifyCodeTxt
        text:"验证码 :"
        font.pixelSize: 14
        anchors.top: passwdTxt.bottom
        anchors.topMargin: emailTxt.anchors.topMargin
        anchors.left: userTxt.left
    }
    TextField{
        id:verifyCodeFld
        anchors.left: verifyCodeTxt.right
        anchors.leftMargin: 10
        anchors.verticalCenter: verifyCodeTxt.verticalCenter
        width: 110
        height: emailTxtFld.height
        Material.accent: "#07C160"
        font.pixelSize: 13
        leftPadding:12
    }
    Text{
        id:verify_hint
        anchors.horizontalCenter: verifyCodeFld.horizontalCenter
        anchors.top: verifyCodeFld.bottom
        anchors.topMargin: 5
        text:""
        font.pixelSize: 12
        color:"black"
        Connections{
            target: NetBridge
            function onVerifySend(success,message){
                if(success){
                    verify_hint.color="green"
                }
                else verify_hint.color="red"
                verify_hint.text=message
            }
        }
    }
    Rectangle{
        id:sendRec
        anchors.left: verifyCodeFld.right
        anchors.leftMargin: 10
        anchors.bottom: verifyCodeFld.bottom
        width: 50
        height: verifyCodeFld.height
        color:"#07C160"
        radius: 4

        Text {
            id: sendTxt
            text: "发送"
            anchors.centerIn: parent
            font.pixelSize: 13
            color:"white"
        }

        MouseArea{
            z:1
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                cursorShape=Qt.PointingHandCursor
                sendRec.color="#06B75B"
            }
            onExited: {
                cursorShape=Qt.ArrowCursor
                sendRec.color="#07C160"
            }
            onClicked: {
                NetBridge.emailRequest(emailTxtFld.text)
            }
        }
    }

    Rectangle{
        id:registerRec
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: leaveTxt.top
        anchors.bottomMargin: 20
        color:"#07C160"
        radius: 4
        width: 180
        height: 36

        Text {
            id: registerText
            text: "注册"
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
                registerRec.color="#06B75B"
            }
            onExited: {
                cursorShape=Qt.ArrowCursor
                registerRec.color="#07C160"
            }
            onClicked: {
                NetBridge.registerRequest(userTxtFld.text,emailTxtFld.text,passwdTxtFld.text,verifyCodeFld.text)
            }
        }
    }
    Text{
        id:register_hint
        anchors.horizontalCenter: registerRec.horizontalCenter
        anchors.top: registerRec.bottom
        anchors.topMargin: 5
        text:""
        font.pixelSize: 12
        color:"black"
        Connections{
            target: NetBridge
            function onRegisterSend(success,message){
                if(success){
                    register_hint.color="green"
                }
                else register_hint.color="red"
                register_hint.text=message
            }
        }
    }

    Text{
        id:loginTxt
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 27
        anchors.right: divid1.left
        anchors.rightMargin: 10
        text:"登入"
        color:"#576B95"
        font.pixelSize: 14
        font.weight: 400

        MouseArea{
            anchors.fill: parent
            onClicked: {
                toenterRec.visible=true
                verifyRec.visible=false
            }
        }
    }

    Rectangle{                                                          //分割线
        id:divid1
        width: 1
        height: 20
        color:"#EBEBEB"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: loginTxt.verticalCenter
        radius: 3
    }

    Text{
        id:leaveTxt
        anchors.verticalCenter: loginTxt.verticalCenter
        anchors.left: divid1.right
        anchors.leftMargin: loginTxt.anchors.rightMargin
        text:"离开"
        color:"#576B95"
        font.pixelSize: 14
        font.weight: 400

        MouseArea{
            anchors.fill: parent
            onClicked: {
                loginRec.visible=true                   //获取上下文了？反正能run（后续可考虑改为信号）
                verifyRec.visible=false
            }
        }
    }
}
