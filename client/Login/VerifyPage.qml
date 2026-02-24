import QtQuick 2.15
import QtQuick.Controls.Material
import Net 1.0
import MyChat 1.0

Rectangle{
    id:verifyRec
    anchors.fill: parent
    visible: false
    color:"transparent"

    Text{
        id:emailTxt
        text:"邮箱 :"
        font.pixelSize: 14
        anchors.top: parent.top
        anchors.topMargin: 130
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
        id:verifyCodeTxt
        text:"验证码 :"
        font.pixelSize: 14
        anchors.top: emailTxt.bottom
        anchors.topMargin: 50
        anchors.left: emailTxt.left
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
        id:hinttxt
        anchors.horizontalCenter: verifyCodeFld.horizontalCenter
        anchors.top: verifyCodeFld.bottom
        anchors.topMargin: 10
        property bool isCorrect: false
        text:isCorrect?"验证成功！":"验证码错误！"
        font.pixelSize: 12
        color:isCorrect?"green":"red"
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

        Connections{
            target: NetBridge
            function onVerifyCode(json){
                console.log("收到验证:",json)
            }
        }

        /*----------------------------------测试用------------------------------------*/
        Connections{
            target: AppState
            function onVerifyEnter(){
                if(verifyCodeFld.text===NetBridge.getCode()&&NetBridge.getCode()!==""){
                    hinttxt.isCorrect=true
                    loadPage.start()
                }
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
        /*----------------------------------测试用------------------------------------*/
    }

    Text{
        id:leaveTxt
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 37
        anchors.left: parent.left
        anchors.leftMargin: 127
        text:"离开"
        color:"#576B95"
        font.pixelSize: 15
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
