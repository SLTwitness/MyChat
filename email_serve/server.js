const grpc=require('@grpc/grpc-js');
const message_proto=require('./proto');
const const_module=require('./const');
const { v4:uudiv4 }=require('uuid');
const emailModule=require('./email');

async function GetVerifyCode(call,callback) {       //await必须在async异步函数中使用
    console.log('email is ',call.request.email);
    try{
        uniqueId=uudiv4();
        console.log('unqiueId is ',uniqueId);
        let verifyId=uniqueId.substring(0,6);           //截取前6位验证码

        let mailOptions_={
            from:'towitness@163.com',
            to:call.request.email,
            subject:'MyChat验证码',
            html:`
            <center>
                <div style="width:700px;font-family: Microsoft Yahei;background-color: #07C160;padding: 10px;border-radius: 6px;">

                    <div style="background-color: white;padding:20px;border-radius: 5px;">
                        <h2 style="color:black;text-align: left;padding-left: 20px;">MyChat验证码：</h2>

                        <div style="background-color: white;height: 30px;"></div>
                        
                        <inline-block style="color: #07C160;font-size: 40px;font-weight: 500;text-align: center;background-color: #EAEAEA;
                            padding-top: 17px;padding-bottom: 17px;padding-left: 70px;padding-right: 70px;border-radius: 4px;">
                            ${verifyId}
                        </inline-block>

                        <div style="background-color: white;height: 40px;"></div>

                        <p style="font-size: 12px;color: black;text-align: left;padding-left: 20px;">
                            请尽快完成注册哦，Ciallo～(∠・ω< )⌒★
                        </p>

                        <br>
                    </div>
                </div>
            </center>
            `
        };

        let send_res=await emailModule.SendMail(mailOptions_);  //await等待promise完成（阻塞）
        console.log("send res is ",send_res);

        /*测试用：返回code*/
        callback(null,{ email:call.request.email , error:const_module.Errors.Success , code:verifyId});
        /*测试用：返回code*/
    }
    catch(error){
        console.log('catch error is ',error);

        callback(null,{ email:call.request.email , error:const_module.Errors.Exception });
    }
}

function main(){
    var server=new grpc.Server();
    server.addService(message_proto.VerifyService.service,{ GetVerifyCode:GetVerifyCode });
    server.bindAsync('0.0.0.0:50051',grpc.ServerCredentials.createInsecure(),()=>{
        console.log('grpc server started');
    })
}

main();