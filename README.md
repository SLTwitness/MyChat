> server和email_server均学习于 [https://gitee.com/secondtonone1/llfcchat  ](https://gitbookcpp.llfc.club/sections/cpp/project/day01.html)  
> 环境配置有所更新，基本全使用当时最新的稳定版本（2025.12）  
> 部分代码稍加修改，如超时检测，邮件等  

&emsp;  
***目前进度：已完成基础的消息收发功能***  
***目前代码量：server:1w+，client:4k+***  
***代码已尽量规范，核心模块注释，客户端图标ps扣的，有帮助可以留个star哦🥰***
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
    - GateServer：基于异步`accept`实现高并发连接接入
    - ChatServer / ChatServer2: 实现异步收发与IO线程池封装
  - Boost.Beast
    - 处理`HTTP`的`GET/POST`请求
  - gRPC+Protobuf
    - GateServer：调用验证码服务，并向状态服务获取`ChatServer`地址和`token`
    - ChatServer / ChatServer2：实现好友申请、好友验证、聊天消息等跨服通知RPC
    - StatusServer：登录调度与`ChatServer`分配
  - JsonCPP
    - 解析与构造`JSON`数据
  - Redis
    - GateServer：验证码缓存、各种key
    - ChatServer：登录计数、会话相关等
  - MySQL
    - 存储用户信息、用户好友、好友请求、聊天业务等数据
- 设计模式：
  - 单例模式
    - 封装通用单例模板，实现`RedisMgr`、`LogicSystem`等核心模块全局管理
- 架构设计：
  - 连接池：封装`gRPC Stub`连接池，复用连接，减轻频繁建连开销，提高调用性能
  - 多线程：主线程跑网络事件轮询，独立线程处理`gRPC`通信，IO线程池负责异步消息收发
  - 分布式架构：通过`GateServer`、`StatusServer`、复数`ChatServer`，实现服务拆分与横向扩展

#### email_serve
- 验证码服务
- 技术栈：
  - Node.js+gRPC
    - 注册验证码并暴露接口供c++调用
  - Redis
    - 验证码缓存
  - html
    - 邮件模板
- 架构设计
  - 异步I/O：`async/await + Promise`封装邮件发送流程，提高并发处理能力
  - 缓存防刷：发送验证码前先查`Redis`，已存在则返回，无则生成，避免重复发信

#### client
- Qt客户端
- 技术栈：
  - CMake
  - Qt 6/QML
  - 信号槽机制
  - 网络模块
- 设计模式：
  - 单例模式
    - 注册全局单例`AppState`、`TcpMgr`等，统一管理状态与网络模块
  - 观察者模式
    - 基于信号与槽实现：网络消息通知、页面联动响应等，实现模块解耦
  - 外观模式
    - QML统一通过调用`Q_INVOKABLE`接口调用功能模块，降低UI与底层实现耦合
  - 中介者模式
    - TcprMgr作为网络消息中介，接收网络数据，并分发业务消息通知`UserMgr`与`QML`，避免模块间直接依赖
- 架构设计
  - MVC架构：
    - M: 后端业务数据
    - V: QML页面
    - C: 网络通信，消息分发等

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
  - 需修改路径等配置信息
  1. 编译server
  2. 在`VerifyServer`文件夹根目录命令行执行`npm run serve`
  3. qt creator编译client
  - 注册：
    - 需编译`VerifyServer`和`GateServer`，并在`VerifyServer`根目录下命令行执行`npm run serve`
  - 登录：
    - 需编译所有server，并提前启动本地redis服务器（因为config.ini中填的是本机地址）

- tips：
  - 注册功能我的163邮箱经常被警告，很可能发不出来
  - 目前部分逻辑有些bug未修

--- 
 
#### 预览图
- 注册功能

![preview](https://github.com/user-attachments/assets/dd79d1f7-9af1-4398-8361-9804a3b2074a)  
（由于与demo改动不大，故不修改预览图了）

- 登入功能（添加好友、认证通过）  
<img width="600" height="338" alt="添加好友 (1)" src="https://github.com/user-attachments/assets/62c3db72-ea9a-4053-be36-05000ecdfde9" />

- 聊天功能  
<img width="600" height="338" alt="聊天通信fin" src="https://github.com/user-attachments/assets/f9880ed0-9438-4fcc-97fe-aa14bd0df141" />
