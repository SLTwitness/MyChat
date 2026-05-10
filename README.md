> server和email_serve均学习于 [https://gitee.com/secondtonone1/llfcchat  ](https://gitbookcpp.llfc.club/sections/cpp/project/day01.html)  
> 环境配置有所更新，基本全使用当时最新的稳定版本（2025.12）  
> 部分代码稍加修改，如超时检测，邮件等  

&emsp;  
***目前进度：验证码服务Demo***  
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

--- 
 
#### 预览图
![preview](https://github.com/user-attachments/assets/dd79d1f7-9af1-4398-8361-9804a3b2074a)
