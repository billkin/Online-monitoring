#ifndef _GDW_I1_2015_H_
#define _GDW_I1_2015_H_

typedef struct
{
	unsigned short Sync;							// 2 报文头： 5AA5
	unsigned short Packet_Length;			// 2 报文长度
	unsigned char CMD_ID[17];					// 17 状态监测装置 ID（17 位编码）
	unsigned char Frame_Type;					// 1 帧类型—参考附表 C8-1 相关含义
	unsigned char Packet_Type;				// 1 报文类型—参考附表 C8-2 相关含义
	unsigned char Frame_No;						// 1 帧序列号（无符号整数）
	unsigned char Payload[];				//报文内容(变长) + 校验位(2位) + 报文尾(0x96) 最大1420
}Frame_StructDef;



typedef struct
{
	unsigned char Component_ID[17];							// 17 被监测设备 ID（17 位编码）
	unsigned int Time_Stamp;										// 4 采集时间
	float Average_WindSpeed_10min;							// 4 10 分钟平均风速（浮点数，精确到小数点后 1 位，单位：m/s）
	unsigned short Average_WindDirection_10min;	// 2 10 分钟平均风向（无符号整数，单位：°）
	float Max_WindSpeed;												// 4 最大风速（浮点数，精确到小数点后 1 位，单位： m/s）
	float Extreme_WindSpeed;										// 4 极大风速（浮点数，精确到小数点后 1 位，单位： m/s）
	float Standard_WindSpeed;										// 4 标准风速（利用对数风廓线转换到标准状态的风速，浮点数，精确到小数点后 1 位，单位： m/s）
	float Air_Temperature;											// 4 气温（浮点数，精确到小数点后 1 位，单位：℃）
	unsigned short Humidity;										// 2 湿度（无符号整数，单位： %RH）
	float Air_Pressure;													// 4 气压（浮点数，精确到小数点后 1 位，单位： hPa）
	float Precipitation;												// 4 降雨量（浮点数，精确到小数点后 1 位，单位： mm）
	float Precipitation_Intensity;							// 4 降水强度（浮点数，精确到小数点后 1 位，单位： mm/min）
	unsigned short Radiation_Intensity;					// 2 光辐射强度（无符号整数，单位： W/m2）
//	unsigned short CRC16;												// 2 校验位
//	unsigned char End;													// 1 报文尾： 0x9
}Meteorological_StructDef;     //气象报文


typedef struct
{
//	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
//	unsigned int Time_Stamp;							// 4 采集时间
	float Inclination;										// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	float Inclination_X;										// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	float Inclination_Y;									// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	float Angle_X;												// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	float Angle_Y;												// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
}I1_TowerTilted_StructDef;   //杆塔倾斜报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned char Unit_Sum;								// 1 采集单元总数（无符号整数， 取值范围： 大于 0）
	unsigned char Unit_No;								// 1 采集单元序号（无符号整数， 取值范围： 大于等于 0）
	unsigned int Time_Stamp;							// 4 采集时间
	unsigned short Strain_Amplitude;			// 2 动弯应变幅值（无符号整数，单位： με ）
	float Bending_Amplitude;							// 4 弯曲振幅（浮点数，精确到小数点后 3 位，单位：mm）
	float Vibration_Frequency;						// 4 微风振动频率（浮点数，精确到小数点后 2 位，单位： Hz）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}BreezeVibrationCharacQuant_StructDef;//微风振动特征量数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned char Unit_Sum;								// 1 采集单元总数（无符号整数， 取值范围： 大于 0）
	unsigned char Unit_No;								// 1 采集单元序号（无符号整数， 取值范围： 大于 0）
	unsigned int Time_Stamp;							// 4 采集时间
	unsigned char SamplePack_Sum;					// 1 数据拆包总数（无符号整数， 取值范围： 大于 0）
	unsigned char SamplePack_No;					// 1 数据报包序（无符号整数， 取值范围： 大于 0）
	unsigned short Strain_Data[];					// 2× N（N 为采样点数） 微风振动信号（无符号整数，单位： με ）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}BreezeVibrationWaveformSignal_StructDef;     //微风振动波形信号数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned int Time_Stamp;							// 4 采集时间
	float Conductor_Sag;									// 4 导线弧垂（浮点数，精确到小数点后 3 位，单位： m）
	float Toground_Distance;							// 4 导线对地距离（浮点数，精确到小数点后 3 位，单位：m）
	float Angle;													// 4 线夹出口处导线切线与水平线夹角（浮点数，精确到小数点后 2 位，单位：°）
	unsigned char Measure_Flag;						// 1 测量法标识： ①0x00 直接法 ②0x01 间接法
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}ConductorSag_StructDef;     //导线弧垂数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned char Unit_Sum;								// 1 采集单元总数（无符号整数， 取值范围： 大于 0）
	unsigned char Unit_No;								// 1 采集单元序号（无符号整数， 取值范围： 大于 0）
	unsigned int Time_Stamp;							// 4 采集时间
	float Line_Temperature;								// 4 线温（浮点数，精确到小数点后 1 位，单位：℃）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}ConductorTemperature_StructDef;     //导线温度数据报文


typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned int Time_Stamp;							// 4 采集时间
	float Equal_IceThickness;							// 4 等值覆冰厚度（浮点数，精确到小数点后 1 位，单位：mm）
	float Tension;												//4 综合悬挂载荷（浮点数，精确到小数点后 1 位，单位：N）
	float Tension_Difference;							//4 不均衡张力差（浮点数，精确到小数点后 1 位，单位：N）
	unsigned char T_Sensor_Num;						// 1 安装的（拉力）传感器个数
	struct
	{
		float Original_Tension;							// 4 第 1 个传感器的原始拉力值（浮点数，精确到小数点后 1 位，单位： N）
		float Windage_Yaw_Angle;						//4 第 1 个传感器的绝缘子串风偏角（浮点数，精确到小数点后 2 位，单位：°）
		float Deflection_Angle;							//4 第 1 个传感器的绝缘子串偏斜角（浮点数，精确到小数点后 2 位，单位：°）
	}Sensor[];    												//N个传感器
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}IcingUnevenTensionDiff_StructDef;     //覆冰及不均衡张力差数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned int Time_Stamp;							// 4 采集时间
	float Windage_Yaw_Angle;							// 4 风偏角（浮点数，精确到小数点后 2 位，单位：°）
	float Deflection_Angle;								// 4 偏斜角（浮点数，精确到小数点后 2 位，单位：°）
	float Least_Clearance;								// 4 最小电气间隙（浮点数，精确到小数点后 3 位，单位： m）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}MonsoonData_StructDef;     //风偏数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned char Unit_Sum;								// 1 采集单元总数（无符号整数， 取值范围： 大于0）
	unsigned char Unit_No;								// 1 采集单元序号（无符号整数， 取值范围： 大于等于 0）
	unsigned int Time_Stamp;							// 4 采集时间
	float U_Gallop_Amplitude;							// 4 舞动幅值（浮点数，精确到小数点后 3 位，单位： m）
	float U_Vertical_Amplitude;						// 4 垂直舞动幅值（浮点数，精确到小数点后 3 位，单位： m）
	float U_Horizontal_Amplitude;					// 4 水平舞动幅值（浮点数，精确到小数点后 3 位，单位： m）
	float U_AngleToVertical;							// 4 舞动椭圆倾斜角（浮点数，精确到小数点后 2位，单位：°）
	float U_Gallop_Frequency;							// 4 舞动频率（浮点数，精确到小数点后 2 位，单位： Hz）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}CharacQuantOfConduDanc_StructDef;     //导线舞动特征量数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned char Unit_Sum;								// 1 采集单元总数（无符号整数， 取值范围： 大于0）
	unsigned char Unit_No;								// 1 采集单元序号（无符号整数， 取值范围： 大于0）
	unsigned int Time_Stamp;							// 4 采集时间
	unsigned char SamplePack_Sum;					// 1 数据拆包总数（无符号整数， 取值范围： 大于0）
	unsigned char SamplePack_No;					// 1 数据报包序（无符号整数， 取值范围： 大于 0）
	struct
	{
		float X; 				//方向相对位移坐标（4Byte）（位移坐标为浮点数，精确到小数点后 3 位，单位： m）
		float Y; 				//方向相对位移坐标（4Byte）
		float Z; 				//方向相对位移坐标（4Byte）
	}Displacement[];			// 12× N（N 为采样点数）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}TraverseTrack_StructDef;     //导线舞动轨迹数据报文

typedef struct
{
	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
	unsigned int Time_Stamp;							// 4 采集时间
	float ESDD;														// 4 等值附盐密度，即盐密（浮点数，精确到小数点后 3位，单位： mg/cm2）
	float NSDD;														// 4 不溶物密度，即灰密（浮点数，精确到小数点后 3 位，单位： mg/cm2）
	float Daily_Max_Temperature;					// 4 日最高温度（浮点数，精确到小数点后 1 位，单位： ℃）
	float Daily_Min_Temperature;					// 4 日最低温度（浮点数，精确到小数点后 1 位，单位： ℃）
	unsigned short Daily_Max_Humidity;		// 2 日最大湿度（无符号整数，单位： ％RH）
	unsigned short Daily_Min_Humidity;		// 2 日最小湿度（无符号整数，单位： ％RH）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}SitePollutionSeverity_StructDef;     //现场污秽度数据报文

typedef struct
{
	unsigned char Request_Set_Flag;				// 1参数配置类型标识： ①0x00 查询配置信息  ②0x01 设置配置信息
	unsigned char Request_Flag;						// 1标识位：字节位自低向高依次对应 9-11 的各选择请求或设置的参数项，
																				// 置 1 表示选择，置 0 表示未选择，如：0x01 表示 IP 被选择
	unsigned char IP[4];									// 4 状态监测装置 IP 地址
	unsigned char Subnet_mask[4];					// 4 子网掩码
	unsigned char Gateway[4];							// 4 网关
	unsigned char PhoneNumber[20];				// 20 手机串号
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}NetworkAdapterQuerySetting_StructDef;     //网络适配器查询/设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;   		// 1 参数配置类型标识： ①0x00 查询配置信息	 ②0x01 设置配置信息
	unsigned char Command_Status;					// 1 数据发送状态： ①0xFF 成功  ②0x00 失败
	unsigned char Request_Flag;						// 1标识位：字节位自低向高依次对应 9-11 的各选择请求或设置的参数项，
																				//置 1 表示选择，置 0表示未选择，如： 0x01 表示 IP 有效
	unsigned char IP[4];									// 4 状态监测装置 IP 地址
	unsigned char Subnet_mask[4];					// 4 子网掩码
	unsigned char Gateway[4];							// 4 网关
	unsigned char PhoneNumber[20];				// 20 手机串号
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}NetworkAdapterQuerySetting_Res_StructDef;     //网络适配器查询/设置数据报文响应

typedef struct
{
	unsigned char Request_Type;						// 1 请求数据类型—参考附表 C8-2 相关含义
	unsigned int Data_Start_Time;					// 4 请求历史数据的开始时间（世纪秒）
	unsigned int Data_End_Time;						// 4 请求历史数据的结束时间（世纪秒）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}UpperLevelDeviceRequestsData_StructDef;    //上级设备请求状态监测装置历史数据

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识： ①0x00 查询配置信息；②0x01 设置配置信息
	unsigned char Request_Flag;						// 1 标识位
	unsigned char Request_Type;						// 1 采样的数据类型—参考附表 C8-2 相关
	unsigned short Main_Time;							// 2 采集时间周期（无符号整数，单位： 分钟）
	unsigned char Heartbeat_Time;					// 1 心跳上送周期（无符号整数，单位： 分钟）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}QuerySetSamplingPeriod_StructDef;     //采样周期查询/设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识： ①0x00 查询配置信息；②0x01 设置配置信息
	unsigned char Command_Status;					// 1	数据发送状态： ①0xFF 成功  ②0x00 失败
	unsigned char Request_Flag;						// 1 标识位
	unsigned char Request_Type;						// 1 采样的数据类型
	unsigned short Main_Time;							// 2 采集时间周期（无符号整数，单位： 分钟）
	unsigned char Heartbeat_Time;					// 1 心跳上送周期（无符号整数，单位： 分钟）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}QuerySetSamplingPeriod_Res_StructDef;     //采样周期查询/设置数据报文响应

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识： ①0x00 查询上位机地址/端口信息 ②0x01 设置上位机地址/端口信息
	unsigned char Request_Flag;						// 1 标识位
	unsigned char IP_Address[4];					// 4 上位机 IP 地址
	unsigned short Port;									// 2 上位机端口号
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}PointsToInformationQuerySettings_StructDef;     //指向信息查询/设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识： ①0x00 查询上位机地址/端口信息 ②0x01 设置上位机地址/端口信息
	unsigned char Command_Status;					// 1数据发送状态： ①0xFF 成功  ②0x00 失败
	unsigned char Request_Flag;						// 1 标识位
	unsigned char IP_Address[4];					// 4 上位机 IP 地址
	unsigned short Port;									// 2 上位机端口号
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}PointsToInformationQuerySettings_Res_StructDef;     //指向信息查询/设置数据报文响应

typedef struct
{
	unsigned char Request_Set_Flag;				// 1参数配置类型标识； ①0x00 查询配置信息 ②0x01 设置配置信息
	unsigned char Request_Flag;						// 1 标识位
	unsigned char Component_ID[17];				// 17 被监测设备新设置的 ID（17 位编码）
	unsigned char Original_ID[17];				// 17 原始 ID，由 7 位厂家编码和 10 位各厂家内部装置编号组成，在出厂时固化，用于设置 ID 时校验
	unsigned char NEW_CMD_ID[17];					// 17 状态监测装置新设置的 ID（17 位编码）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}ID_QuerySetting_StructDef;     //ID 查询及设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识； ①0x00 查询配置信息  ②0x01 设置配置信息
	unsigned char Command_Status;					// 1 数据发送状态： ①0xFF 成功 ②0x00 失败
	unsigned char Request_Flag;						// 1 标识位
	unsigned char Component_ID[17];				// 17 被监测设备当前的 ID（17 位编码）
	unsigned char Original_ID[17];				// 17 原始 ID
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}ID_QuerySetting_Res_StructDef;     //ID 查询及设置数据报文响应

typedef struct
{
	unsigned char Reset_Mode;							// 1 装置复位： ①0x00 常规复位（重启）；②0x01 复位至调试模式。
																				// 在调试模式下，状态监测装置保持通信状态，不休眠，依次发送各种报文，
																				//同时接收各种命令，调试模式最多持续 10 分钟。
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}ResetDevice_StructDef;     //装置复位数据报文

typedef struct
{
	unsigned char Request_Set_Flag;				// 1参数配置类型标识： ①0x00 查询配置信息；②0x01 设置配置信息
	unsigned char Config_Total;						// 1 配置参数个数
	struct
	{
		unsigned char Name[6];				// 6 第一个配置参数的名称（字符串， 取值范围： 小写字母与数字）
		unsigned char DataType;				// 1 第一个配置参数的数据类型（无符号整数）：①0x00 无符号整数；②0x01 有符号整数；3 0x02 单精度浮点数
		unsigned int 	Data;						// 4 第一个配置参数的值
	}Config[];
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}QuerySetConfigurationInformation_StructDef;     //配置信息查询/设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;			// 1 参数配置类型标识： ①0x00 查询配置信息；②0x01 设置配置信息
	unsigned char Command_Status;				// 1 数据发送状态： ①0xFF 成功 ②0x00 失败
	unsigned char Config_Total;					// 1 配置参数个数
	struct
	{
		unsigned char Name[6];			// 6 第n个配置参数的名称（字符串， 取值范围： 小写字母与数字）
		unsigned char DataType;			// 1 第n个配置参数的数据类型（无符号整数）：①0x00 无符号整数；②0x01 有符号整数；3 0x02 单精度浮点数
		unsigned int  Data;			    // 4 第n个配置参数的值
	}Config[];
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}QuerySetConfigurationInformation_Res_StructDef;     //配置信息查询/设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;		// 1 参数配置类型标识； ①0x00 查询配置信息 ②0x01 设置配置信息
	unsigned char Request_Flag;				// 1 标识位：字节位自低向高依次对应 9-11 的各选择请求或设置的参数项，
																		// 置 1 表示选择，置 0 表示未选择，如： 0x01 表示 IP 被选择
	unsigned char Color_Select;				// 1 色彩选择： 0 为黑白， 1 为彩色
	unsigned char Resolution;					// 1 自定义图像分辨率，采用以下几组：
																		// ①320 X 240 为 1；
																		// ②640 X 480 为 2；
																		// ③704 X 576 为 3；
																		// ④720 X 480 为 4； （标清）
																		// ⑤1280 X 720 为 5；（720P）
																		// ⑥1920 X 1080 为 6；（1080P）
																		// ⑦1280P 或者更高为 7。
	unsigned char Luminance;					//  1 亮度（无符号整数， 取值范围： 1～100）
	unsigned char Contrast;						// 1 对比度（无符号整数， 取值范围： 1～100）
	unsigned char Saturation;					// 1 饱和度（无符号整数， 取值范围： 1～100）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}QuerySetImageAcquisitionParameters_StructDef;     //图像采集参数查询/设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;		// 1 参数配置类型标识； ①0x00 查询配置信息 ②0x01 设置配置信息
	unsigned char Command_Status;			// 1 数据发送状态： ①0xFF 成功 ②0x00 失败
	unsigned char Request_Flag;				// 1 标识位
	unsigned char Color_Select;				// 1 色彩选择： 0 为黑白， 1 为彩色
	unsigned char Resolution;					// 1 自定义图像分辨率，采用以下几组：
																		// ①320 X 240 为 1；
																		// ②640 X 480 为 2；
																		// ③704 X 576 为 3；
	unsigned char Luminance;					// 1 亮度（无符号整数， 取值范围： 1～100）
	unsigned char Contrast;						// 1 对比度（无符号整数， 取值范围： 1～100）
	unsigned char Saturation;					// 1 饱和度（无符号整数， 取值范围： 1～100）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}QuerySetImageAcquisitionParameters_Res_StructDef;     //图像采集参数查询/设置数据报文响应

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识； ①0x00 查询配置信息 ②0x01 设置配置信息
	unsigned char Channel_No;							// 1 通道号？ —表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Group;									// 1 组数—后面所配置的时间组数目，不大于 72 组
	struct
	{
		unsigned char Hour;									// 1 时 1
		unsigned char Minute;								// 1 分 1
		unsigned char Presetting_No;				// 1 预置位号 1—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	}GroupSet[];
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}PhotoScheduleSetting_StructDef;     //拍照时间表设置数据报文

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 参数配置类型标识； ①0x00 查询配置信息 ②0x01 设置配置信息
	unsigned char Channel_No;							// 1 通道号？ —表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char	Command_Status;					// 1 数据发送状态： ①0xFF 成功 ②0x00 失败
	unsigned char Group;									// 1 组数—后面所配置的时间组数目，不大于 72 组
	struct
	{
		unsigned char Hour;									// 1 时 1
		unsigned char Minute;								// 1 分 1
		unsigned char Presetting_No;				// 1 预置位号 1—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	}GroupSet[];
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}PhotoScheduleSetting_Res_StructDef;     //拍照时间表设置数据报文响应

typedef struct
{
	unsigned char Channel_No;					// 1 通道号—表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Presetting_No;			// 1 预置位号—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}ManuallyRequestphoto_StructDef;     //手动请求拍摄照片数据报文


typedef struct
{
	unsigned char Channel_No;				// 1 通道号—表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Presetting_No;		// 1 预置位号—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	unsigned char Packet_High;			// 1 包数高位（无符号整数）
	unsigned char Packet_Low;				// 1 包数低位（无符号整数）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}DeviceRequestedPhotoUpload_StructDef;     //装置请求上送照片数据报文

typedef struct
{
	unsigned char Channel_No;				// 1 通道号—表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Presetting_No;		// 1 预置位号—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	unsigned short Packet_No;				// 2 总包数（无符号整数， 取值范围： 大于等于 0）
	unsigned short Subpacket_No;		// 2 子包包号（无符号整数， 取值范围： 大于等于 1）
	unsigned char Sample[];			// N 数据区（除最后一包外最小为 512）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}RemoteImageData_StructDef;     //远程图像数据报文

typedef struct
{
	unsigned char Channel_No;			// 1 通道号—表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Presetting_No;	// 1 预置位号—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	unsigned int Time_Stamp;			// 4 时间
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}RemoteImageData_SendEndMark_StructDef;     //远程图像数据上送结束标记报文

typedef struct
{
	unsigned char Channel_No;								// 1 通道号—表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Presetting_No;						// 1 预置位号—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	unsigned short ComplementPack_Sum;			// 2 补包包数—未收到的总包数（无符号整数）
	unsigned short ComplementPack_No[];		// 2× N 补包包号序列 
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}RemoteImagePatchDataDelivery_StructDef;     //远程图像补包数据下发报文

typedef struct
{
	unsigned char Channel_No;							// 1 通道号—表示采集装置上的摄像机编号。如： 一个装置连接 2 部摄像机， 则分别标号为 1、 2
	unsigned char Presetting_No;					// 1 预置位号—即云台摄像所设置的预置位号，不带云台摄像机，预置位号为 255
	unsigned char Action;									// 1 动作指令—意义见附表 C8-3 动作指令表
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}RemoteCameraAdjustment_StructDef;     //摄像机远程调节报文

typedef struct
{
	unsigned int Clocktime_Stamp;					// 4 当前时间
	float Battery_Voltage;								// 4 电源电压（浮点数，精确到小数点后 1 位，单位： V）
	float Operation_Temperature;					// 4 工作温度（浮点数，精确到小数点后 1 位，单位：℃）
	float Battery_Capacity;								// 4 电池剩余电量（浮点数，精确到小数点后 1 位，单位： Ah）
	unsigned char FloatingCharge;					// 1 浮充状态： ①0x00 充电  ②0x01 放电
	unsigned int Total_Working_Time;			// 4 工作总时间（无符号整数，单位： 小时）
	unsigned int Working_Time;						// 4 本次连续工作时间（无符号整数，单位： 小时）
	unsigned char Connection_State;				// 1 连接状态： ①0x00 与所有传感器连接正常 ②0x01 与一个或者多个传感器连接异常
	unsigned int Send_Flow;								// 4 当月发送流量（无符号整数，单位：字节）
	unsigned int Receive_Flow;						// 4 当月接收流量（无符号整数，单位：字节）
	unsigned int Protocol_Version;				// 4 通信协议版本号（带小数位）四个部分，每个字节代表一段；举例： 版本号 1.2.4.10 的 4 个字节表示为： 01 02 04 0A
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}HeartbeatPacket_StructDef;     //心跳数据报文

typedef struct
{
	unsigned char Command_Status;					// 1 数据发送状态： ①0xFF 成功 ②0x00 失败
	unsigned char Mode;										// 1 运行模式： ①0x00 切换到正常模式 ②0x01 切换到调试模式
	unsigned int Clocktime_Stamp;					// 4 上位机当前时间（世纪秒，当值为 0 时，表示装置时间与上位机时间一致；当值非 0 时，装置需要根据该时间进行校时）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}HeartbeatPacket_Res_StructDef;     //心跳数据报文响应

typedef struct
{
	unsigned int Time_Stamp;							// 4 采集时间
	unsigned char Fault_Desc[512];				// N（N<512Byte） 返回装置故障描述信息（UTF-8 编码）
//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
}DeviceFaultInformation_StructDef;     //装置故障信息数据报文


typedef struct
{
	unsigned char File_Name[20];		// 20 文件名，以’\0’结尾 ASCII字符串，占用20Byte,不足 20字节的补’\0’
	unsigned int File_Size;					// 4 文件长度(低字节在前)
  unsigned short Packet_No;				// 2 总包数(低字节在前)
	unsigned short Packet_Size;			// 2 包长度(低字节在前)
}RemoteUpgradeStart_StructDef;    //远程升级开始报文 ,应答报文格式一样，原封不动的响应


typedef struct
{
	unsigned short File_Packet;			// 2 总包号
	unsigned short Packet_No;				// 2 包序号(包号从 1开始递增)
  unsigned short Packet_Len;			// 2 包数据长度
	unsigned char  File_Content[];	// Packet_Len 文件内容
}RemoteUpgradeData_StructDef;     //远程升级数据报文

typedef struct
{
	unsigned char File_Name[20];		// 20 文件名，以’\0’结尾 ASCII字符串，占用20Byte,不足 20字节的补’\0’
	unsigned int File_Size;					// 4 文件长度(低字节在前)
}RemoteUpgradeEnd_StructDef;      //远程升级数据报下发结束标记

typedef struct
{
	unsigned short Upload_Result;				// 2  操作结果(低字节序)
	unsigned char File_Name[20];				// 20 文件名，以’\0’结尾 ASCII字符串，占用20Byte
	unsigned short ComplementPack_Sum;	// 2 补包包数—未收到的总包数(低字节序)
	unsigned int ComplementPack_No;			// 4×N 补包包号序列(低字节序)
}ComplementPack_StructDef;        		//软件补包数据报文


typedef struct
{
	unsigned char Ctrl_Cmd;							 // 1   0:常规复位 1:复位至升级模式 2:复位至应用程序模式
}SoftwareUpgradeCtrlCmd_StructDef;     //软件升级控制命令

typedef struct
{
	unsigned char Ctrl_Result;							// 1  0:成功 其它:失败
}SoftwareUpgradeCtrlCmd_Res_StructDef;     //软件升级控制命令响应报文

//获取版本号报文无报文内容
typedef struct
{
	unsigned char version[20];					// 20 当前版本号不够’\0’补齐
	unsigned char mode;									// 1 模式
	unsigned char State[20];						// 20 异常状态
}ObtainVersionNumber_Res_StructDef;   //获取版本号响应报文


//typedef struct
//{
//	unsigned char Component_ID[17];				// 17 被监测设备 ID（17 位编码）
//	unsigned int Time_Stamp;							// 4 采集时间

//	unsigned short CRC16;									// 2 校验位
//	unsigned char End;										// 1 报文尾： 0x9
//}_StructDef;     //报文

//	if(frame[0]==0xA5&&frame[1]==0x5A)
//	{
//		Packet_Length= (frame[3]<<8) + frame[2];
//		pGDW_Frame = (Frame_StructDef*)malloc(sizeof(Frame_StructDef)+(Packet_Length+3)*sizeof(unsigned char));//变长分配内存: 报文内容(变长) + 校验位(2位) + 报文尾(1位) 最大1420
//		p=(unsigned char*)pGDW_Frame;
//		for(i=0;i<frame_length;i++) p[i]=frame[i];
//		
//	}

//#define USE_Meteorological 			1		//气象报文
//#define USE_TowerTilted					1   //杆塔倾斜报文

//#if (USE_Meteorological)
//	extern Meteorological_StructDef Meteorological;     //气象报文
//#endif

//#if (USE_TowerTilted)
//	extern TowerTilted_StructDef TowerTilted;   				//杆塔倾斜报文
//#endif

//extern NetworkAdapterQuerySetting_StructDef  *pNetworkAdapterQuerySetting;     //网络适配器查询/设置数据报文


//unsigned short RTU_CRC(unsigned char * puchMsg,unsigned short usDataLen);  //CRC 校验算法
//void GDW_Protocol_test(unsigned short Packet_Length);
//void FrameAnalysis(Frame_StructDef *frame);	
//unsigned char CMD_to_CMA(unsigned char Frame_Type,unsigned char Packet_Type,unsigned char *pdata,unsigned short len);

#endif

