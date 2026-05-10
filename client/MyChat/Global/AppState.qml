pragma Singleton
import QtQuick

QtObject{
    //signal verifyEnter()       //测试用：进入MyChat
    signal loginSuccess()       //测试用:登入成功，进入MyChat

    signal searchFocus()    //取消搜索的焦点（不仅限于搜索）

    signal showHome()       //显示图标
    signal showPage()           //在leftpage显示空白（显示别的页面，当marginPage选择不同时的信号）
    signal showChatPage()           //在leftpage显示chat列表
    signal showFriendPage()         //在leftpage显示好友列表

    signal showChat(int uid,string name,string icon)        //传递在chat列表中选中的联系人名字给rightpage
    signal showFriendApply(int uid,string name,int sex,string img,int msg)  //传递在fiendApply列表中选中的好友申请给rightpage
    signal showFriend(int uid,string name,int sex,string img)       //传递在联系人列表中选中的好友申请给rightpage

    property int searchUserUid
    property int searchUserSex
    property string searchUserName
    property string searchUserIcon
}
