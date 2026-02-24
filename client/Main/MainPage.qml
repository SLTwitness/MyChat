import QtQuick 2.15
import MyChat 1.0
import Qt5Compat.GraphicalEffects

Item{
    id:mainPage

    //最后渲染底层的拖动区域（Qt从底向上渲染）
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        z:0
        onPressed: {
            mainWindow.startSystemMove()
            AppState.searchFocus()
        }
    }

    MarginPage{
        id:marginPage
    }

    LeftPage{
        id:leftPage
    }

    RightPage{
        id:rightPage
    }

}
