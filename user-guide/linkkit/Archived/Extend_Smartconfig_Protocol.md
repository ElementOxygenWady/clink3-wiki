# <a name="目录">目录</a>
+ [设计背景](#设计背景)
+ [设计简述](#设计简述)
+ [编码方案](#编码方案)
    * [带宽节约型的tlv数据结构](#带宽节约型的tlv数据结构)
    * [现有的一键配网协议格式](#现有的一键配网协议格式)
    * [扩展的一键配网协议格式](#扩展的一键配网协议格式)

# <a name="设计背景">设计背景</a>

当前一键配网受限于原有格式, 未能传输ssid/passwd以外的信息. 本方案拟改造这一点, 增加token等字段的传输

# <a name="设计简述">设计简述</a>

+ 修改原有的一键配网协议, 使其能够传输包含token等字段的更多的信息

# <a name="编码方案">编码方案</a>

## <a name="带宽节约型的tlv数据结构">带宽节约型的tlv数据结构</a>

> 为提高方案的可扩展性, 便于以后升级维护, 本方案使用tlv方式传递可变的数据

+ Type-Length合并为一个字节, 前面3个bits表示type,后面5个表示length
+ Type字段的3个bit表示
    - 000: 表示BSSID类型
    - 001: 表示IP类型
    - 其他值未定义

## <a name="现有的一键配网协议格式">现有的一键配网协议格式</a>

| 1B      | 1B      | 1B          | 1B          | NB      | MB      | 2B
|---------|---------|-------------|-------------|---------|---------|-------------
| Tlen    | flag    | ssid_len    | passwd_len  | ssid    | passwd  | checksum


## <a name="扩展的一键配网协议格式">扩展的一键配网协议格式</a>
| 1B      | 1B      | 1B          | 1B          | NB      | MB      | 2B          | 1B      | 1B              | NB      | 1B
|---------|---------|-------------|-------------|---------|---------|-------------|---------|-----------------|---------|-------------
| Tlen    | flag    | ssid_len    | passwd_len  | ssid    | passwd  | checksum    | Tlen2   | token_type_len  | token   | checksum2


各字段说明
---
| 字段                | 说明
|---------------------|---------------------------------------------------------------------------------------------------------------------
| `flag`              | flag的第二高位(0b01000000)如果为1, 表示这是带附加一键配网协议的. 如果为0, 表示这是不附带一键配网协议的
| `Tlen2`             | 附加协议的总长度(total length)
| `token_type_len`    | 采用了上述的tlv结构, 高三位表示类型(001表示token), 低五位表示bssid的长度(暂定3B). 所以这个字段值为0b00100011=0x23
| `token`             | token中的三个字符
| `checksum2`         | 1个byte,表示当前所有传输信息的checksum

---
*上述格式中"NB/MB"表示可变的Byte数*


备注
---
+ app如果升级后通过tlv形式下推更多的字段, 老版本的设备也能兼容
+ app端发出的包长都为1, 以此作为配网包的标记.
+ 设备端的16个字节的token, 需要根据app传来的3个字节的token和wifi的密码来一起生成
