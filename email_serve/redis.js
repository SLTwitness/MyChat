const config_module=require('./config')
const Redis=require('ioredis')

//创建redis客户端实例
const RedisCli=new Redis({
    host:config_module.redis_host,
    port:config_module.redis_port,
    password:config_module.redis_passwd
})

//监听报错
RedisCli.on('error',function(err){
    console.log('RedisCli connect error')
    RedisCli.quit()
})

//获取key对应的val
async function getRedis(key) {
    try{
        const res=await RedisCli.get(key)   //依旧await异步改同步
        if(res==null){
            console.log('result:','<'+res+'>',',this key cant be found')
            return null
        }
        console.log('result:','<'+res+'>',',get key succeed')
        return res
    }catch(error){
        console.log('getRedis error is ',error)
        return null
    }
}

//查询是否存在key
async function existKey(key) {
    try{
        const res=await RedisCli.exists(key)
        if(res==0){
            console.log('result:<','<'+res+'>',',this key is null')
            return null
        }
        console.log('result:<','<'+res+'>',',with this value')
        return res
    }catch(error){
        console.log('existKey error is ',error)
        return null
    }
}

//设置key和val，且过期时间
async function setExpire(key,val,outime) {
    try{
        await RedisCli.set(key,val)
        await RedisCli.expire(key,outime)
        return true
    }catch(error){
        config.log('setExpire error is ',error)
        return false
    }
}

//退出
function Quit(){
    RedisCli.quit()
}

module.exports={getRedis,existKey,setExpire,Quit}

