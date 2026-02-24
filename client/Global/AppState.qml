pragma Singleton
import QtQuick

QtObject{
    signal verifyEnter()       //测试用：进入MyChat
    signal loginSuccess()       //测试用:验证通过

    signal searchFocus()    //取消搜索的焦点（不仅限于搜索）

    signal showHome()       //显示图标
    signal showPage()           //在leftpage显示空白（显示别的页面）
    signal showChatPage()           //在leftpage显示chat列表
    signal showChat(string name)        //传递在chat列表中选中的联系人名字给rightpage
}
