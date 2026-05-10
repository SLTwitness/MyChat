#ifndef ERRORCODE_H
#define ERRORCODE_H

enum errorCode{
    Success = 0,
    Error_Json = 1001,
    RPCFailed = 1002,

    VerifyExpired = 1003,
    VerifyCodeErr = 1004,
    UserExist = 1005,

    PasswdInvalid = 1006,
    UidInvalid = 1007,
    TokenInvalid = 1008
};

#endif // ERRORCODE_H
