const nodemailer=require('nodemailer');
const config_module=require('./config');

/*
创建邮箱发送的代理
*/
let transport=nodemailer.createTransport({
    host:'smtp.163.com',
    port:465,
    secure:true,
    auth:{
        user:config_module.email_user,
        pass:config_module.email_pass
    }
});

/*
发送邮件的函数
@param{*} mailOptions_ 发送邮件的参数
@returns
*/
function SendMail(mailOptions_){
    return new Promise(function(resolve,reject){        //类似于c++的future（异步转同步）
        transport.sendMail(mailOptions_,function(error,info){   //传递发送的选项，和回调函数（异步）
            if(error){
                console.log(error);
                reject(error);
            }
            else{
                console.log('已成功发送邮件: '+info.response);
                resolve(info.response);                 //resolve,reject任一参数被调用即promise完成
            }
        });
    });
}

module.exports.SendMail=SendMail;