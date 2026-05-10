> server和email_serve均学习于 [https://gitee.com/secondtonone1/llfcchat  ](https://gitbookcpp.llfc.club/sections/cpp/project/day01.html)  
> 环境配置有所更新，基本全使用当时最新的稳定版本（2025.12）  
> 部分代码稍加修改，如超时检测，邮件等  

&emsp;  
***目前进度：已完成基础的消息收发功能***  
***代码已尽量规范，部分注释，客户端图标ps扣的，有帮助可以留个star哦🥰***
&emsp;  

- [项目内容](#项目内容)
- [本地部署](#本地部署)
- [预览图](#预览图)

---

### 项目内容
#### server  
- c++服务器
- 技术栈：
  - Boost.Asio网络库
    - 实现异步服务器
  - gRPC
    - 调用验证码服务
  - JsonCPP
    - 解析POST请求返回的JSON文件
- 设计模式：
  - 单例模式
    - 懒汉式线程安全
- MVC架构
  - C：路由分发

#### email_serve
- 验证码服务
- 技术栈：
  - gRPC微服务
    - 暴露接口供c++调用
  - 异步发送邮件
  - html
    - 邮件页面
- MVC架构
  - M

#### client
- Qt客户端
- 技术栈：
  - cmake
  - 信号槽
  - 网络模块
- 设计模式：
  - 单例模式
    - 注册单例AppState，管理全局状态信号等
  - 观察者模式
    - 接收信号并反应
- MVC架构：
  - V

--- 

#### 本地部署
- 环境要求
  - Visual Studio 2022
  - boost_1_89_0
  - jsoncpp 1.9.6
  - grpc v1.41（这个很奇怪，我记得是2026.1直接clone的，现在describe --tags发现版本很旧）
  - cmake v4.2.1
  - nasm v3.01
  - node.js v24.13.0
  - qt 6.8.3
  - qt creator
- 部署编译
  - 应该只有server不能够直接编译，需要将我配置的路径全都改掉
  1. 编译server
  2. 在email_serve文件夹根目录命令行执行`npm run serve`
  3. qt creator编译client
  - 注册：
    - 需编译verifyserver和gateserver，并在verifyserver根目录下命令行执行`npm run serve`
  - 登录：
    - 需编译所有server，并提前启动本地redis服务器（因为config.ini中填的是本机地址）

- tips：
  - 注册功能我的163邮箱经常被警告，很可能发不出来

--- 
 
#### 预览图
- 注册功能

![preview](https://github.com/user-attachments/assets/dd79d1f7-9af1-4398-8361-9804a3b2074a)  
（由于与demo改动不大，故不修改预览图了）

- 登入功能  
<img width="600" height="338" alt="添加好友 (1)" src="https://github.com/user-attachments/assets/62c3db72-ea9a-4053-be36-05000ecdfde9" />

- 聊天功能  
<img width="600" height="338" alt="聊天通信fin" src="https://github.com/user-attachments/assets/f9880ed0-9438-4fcc-97fe-aa14bd0df141" />
