#ifndef PROXY_PROTO_H
#define PROXY_PROTO_H

#define MAXLINE     1024        //每个udp包的数据大小


#define MAX_FILE    20           //每个应用传输队列的最大值
#define MAX_SPEED   800          //最大传输速率
#define PACK_T		200

#define CMD_REGISTER     0x00    //注册应用
#define CMD_DELECT       0x01    //删除应用
#define CMD_QUERY        0x02	 // 查询服务
#define CMD_ADD_FILE     0x10    //添加文件到指定应用的发送队列


// 错误码
#define E_OK            0x00	// 成功
#define E_EXIST         0x01    // 应用已存在 （注册）
#define E_NOTFOUND      0x02    // 应用不存在 （删除）
#define E_FULL          0x03    // 发送队列满
#define E_BAD_CMD       0x04    // 错误的参数
#define E_THREAD        0x05    // 创建传输线程失败
#define E_REGISTER      0x06    // 注册应用失败
#define E_FORMAT        0x07    // 消息格式错误
#define CMD_E_UKNKNOW	0xFF	//未知的错误

//文件传输线程状态
#define S_IDLE          0x00    //线程空闲
#define S_BUSY          0x01    //线程运行中
#define S_WAIT          0x02    //带宽占满，需要等待
#define S_CANCEL        0x03    //线程取消

#define S_ERROR         0xff    //线程错误

//删除应用的策略
#define DEL_NOW         0       //立刻终止服务，不管队列里是否还有文件未传输完成
#define DEL_LAST_ONE    1       //如果队列有未完成的文件，传输完当前文件后终止
#define DEL_LATER       2       //不能再添加文件到队列，传输完所有文件后停止


/*******************************************************/
//[1] add for config file
#include "config.h"
#include "Log.h"

//define server mode
#define     CLIENT_MODE     0
#define     SERVER_MODE     1

#define DF_CFG      "/usr/share/udp-file-proxy/etc/udp-file-proxy.cfg"
#define DF_LOG_FILE "/usr/share/udp-file-proxy/log/log.txt"
extern class CONF g_conf;



#endif // PROXY_PROTO_H

