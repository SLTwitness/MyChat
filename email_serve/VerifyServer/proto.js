const path=require('path');      //相当于包含头文件
const grpc=require('@grpc/grpc-js');
const protoLoader=require('@grpc/proto-loader');

const PROTO_PATH=path.join(__dirname,'message.proto');
const packageDefinition=protoLoader.loadSync(PROTO_PATH,{keepCase:true,longs:String,
    enums:String,defaults:true,oneofs:true});           //解析proto

const protoDescriptor=grpc.loadPackageDefinition(packageDefinition);        //转为grpc对象
const message_proto=protoDescriptor.message;        //从中取出message包

module.exports=message_proto;