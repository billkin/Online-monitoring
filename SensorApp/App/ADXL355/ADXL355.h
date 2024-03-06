#ifndef __ADXL355_H__
#define __ADXL355_H__

#include "main.h"
#include "spi.h"

#define pi_ang 	57.295779513			//180/π
#define PI      3.1415926

#define XL355_hspi	hspi2
#define XL355_SPI_CS 			ADXL355_CS_Pin
#define XL355_SPI_CS_PORT 	ADXL355_CS_GPIO_Port

////IO操作函数											   

#define XL355_DEVID_AD			0x00	//R	ADI ID 寄存器,此寄存器包含 ADI 公司 ID 0xAD
#define XL355_DEVID_MST			0x01	//R ADI MEMS ID 寄存器,此寄存器包含 ADI 公司 MEMS ID 0x1D	
#define XL355_PARTID				0x02	//R	器件 ID 寄存器,此寄存器包含器件 ID 0xED（八进制 355）
#define XL355_REVID					0x03	//R	产品版本 ID 寄存器,此寄存器包含产品版本 ID，从 0x00 开始，随后每次修订均递增 1
#define XL355_STATUS				0x04	//R	状态寄存器,该寄存器包括反映 ADXL355 各种状况的位。
#define XL355_FIFO_ENTRIES	0x05	//R	FIFO 条目寄存器该寄存器,表示 FIFO 缓冲器中存在的有效数据样本的数量。此数量的范围是 0 到 96
#define XL355_TEMP2					0x06	//R	温度数据寄存器,TEMP2 包含 12 位值的 4 个最高有效位
#define XL355_TEMP1					0x07	//R	TEMP1 包含其 8 个最低有效位
#define XL355_XDATA3				0x08	//R	X轴数据寄存器,这三个寄存器包含X轴加速度数据。数据为左对齐，采用二进制补码格式。
#define XL355_XDATA2				0x09
#define XL355_XDATA1				0x0A
#define XL355_YDATA3				0x0B	//R	Y轴数据寄存器,这三个寄存器包含Y轴加速度数据。数据为左对齐，采用二进制补码格式。
#define XL355_YDATA2				0x0C
#define XL355_YDATA1				0x0D
#define XL355_ZDATA3				0x0E	//R	Z轴数据寄存器,这三个寄存器包含Z轴加速度数据。数据为左对齐，采用二进制补码格式。
#define XL355_ZDATA2				0x0F
#define XL355_ZDATA1				0x10
#define XL355_FIFO_DATA			0x11	//R	FIFO访问寄存器
#define XL355_OFFSET_X_H		0x1E	//R/W	X轴偏移,X轴失调调整寄存器
#define XL355_OFFSET_X_L		0x1F	
#define XL355_OFFSET_Y_H		0x20	//R/W	Y轴偏移,Y轴失调调整寄存器
#define XL355_OFFSET_Y_L		0x21
#define XL355_OFFSET_Z_H		0x22	//R/W	Z轴偏移,Z轴失调调整寄存器
#define XL355_OFFSET_Z_L		0x23
#define XL355_ACT_EN				0x24	//R/W	活动使能寄存器
#define XL355_ACT_THRESH_H	0x25	//R/W	活动阈值寄存器,ACT_TRESH[15:0]
#define XL355_ACT_THRESH_L	0x26
#define XL355_ACT_COUNT			0x27	//R/W	活动计数寄存器,检测活动所需的超过阈值的连续事件数
#define XL355_FILTER				0x28	//R/W	滤波器设置寄存器,指定内部高通和低通滤波器的参数
#define XL355_FIFO_SAMPLES	0x29	//R/W	FIFO 样本寄存器,使用 FIFO_SAMPLES 值指定 FIFO 中存储的样本数。该寄存器的默认值为 0x60，以避免触发 FIFO 水印中断
#define XL355_INT_MAP				0x2A	//R/W	中断引脚(INTx)功能映射寄存器
#define XL355_SYNC					0x2B	//R/W	数据同步,使用该寄存器控制外部定时触发器。
#define XL355_RANGE					0x2C	//R/W	I2C 速度、中断极性和范围寄存器
#define XL355_POWER_CTL			0x2D	//R/W	电源控制寄存器,省电功能控制
#define XL355_SELF_TEST			0x2E	//R/W	自检寄存器
#define XL355_RESET					0x2F	//W	复位寄存器,写入代码 0x52 即复位器件，类似于上电复位(POR)

/* Temperature parameters */
#define ADXL355_TEMP_BIAS       (float)1852.0      /* Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /* Accelerometer temperature change from datasheet (LSB/degC) */

#pragma pack (1) /*指定按1字节对齐*/

struct ADXL355_STATUS_DEF
{
	uint8_t DATA_RDY : 	1;	//0 DATA_RDY 进行了完整的 x 轴、 y 轴和 z 轴测量，并且可以读取结果。 0x0 R
	uint8_t FIFO_FULL: 	1;	//1 FIFO_FULL 达到 FIFO 水印。 0x0 R
	uint8_t FIFO_OVR : 	1;	//2 FIFO_OVR FIFO 溢出，最旧的数据丢失。 0x0 R
	uint8_t ACT 		 : 	1;	//3 活动 检测到 THRESH_ACT 和 COUNT_ACT 寄存器所定义的活动。 0x0 R
	uint8_t NVM_BUSY : 	1;	//4 NVM_BUSY NVM 控制器忙于刷新、编程或内置自检(BIST)。 0x0 R
	uint8_t Reserve  : 	3	;	//[7:5] 保留 保留。 0x0 R
};

struct ADXL355_Typedef
{
	float	x_ang;
	float y_ang;
	float z_ang;
	float Temp;
	double	ax;
	double  ay;
	double	az;
	float Scale;
	struct ADXL355_STATUS_DEF Status;
};

//传感器复位值，偏移值
struct ADXL355_XYZ_Offset_Def
{
	int16_t	X_Offset;
	int16_t Y_Offset;
	int16_t	Z_Offset;
//	int16_t X_acc_Offset;
//	int16_t Y_acc_Offset;
//	int16_t Z_acc_Offset;
};

#pragma pack ()

/**************************** Configuration parameters **********************/
/* Temperature parameters */
#define ADXL355_TEMP_BIAS       (float)1852.0      /* 加速度计温度偏差(在ADC代码中)在25℃ Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /*加速计温度变化数据表(LSB/degC) Accelerometer temperature change from datasheet (LSB/degC) */

/* Accelerometer parameters */
#define ADXL_RANGE     2     /* ADXL362 sensitivity: 2, 4, 8 [g] */

#define ACT_VALUE          50     /* Activity threshold value */

#define INACT_VALUE        50     /* Inactivity threshold value */

#define ACT_TIMER          100    /* Activity timer value in ms */

#define INACT_TIMER        10     /* Inactivity timer value in seconds */

/****************************** Global Data ***********************************/

//extern volatile int32_t i32SensorX;
//extern volatile int32_t i32SensorY;
//extern volatile int32_t i32SensorZ;

//extern volatile uint32_t ui32SensorX;
//extern volatile uint32_t ui32SensorY;
//extern volatile uint32_t ui32SensorZ;
//extern volatile uint32_t ui32SensorT;

//extern volatile uint32_t ui32timer_counter;
 
 
/*************************** Functions prototypes *****************************/

void ADXL355_Init(void);
void ADXL355_Start_Sensor(void);
void ADXL355_Stop_Sensor(void);
void ADXL355_Data_Scan(struct ADXL355_Typedef *pADXL355);
int32_t ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData);
void ADXL355_Offset_RW(uint8_t axle,int16_t *Val,uint8_t mode);							//XYZ 轴失调调整寄存器读写.
void ADXL355_ACT_THRESH_RW(uint16_t *ACT_Val,uint8_t *ACT_Count,uint8_t mode);//活动阈值,活动计数寄存器寄存器读写.
void ADXL355_ACT_EN_W(uint8_t axle_bit); //活动使能寄存器
struct ADXL355_STATUS_DEF ADXL355_Status_Read(void); //读取状态
float ADXL355_Read_Temp(void);       //读取温度
	
void Shell_ADXL355_ACC_Read(struct ADXL355_Typedef *pADXL355);
void ADXL355_ACT_Set(void); //
void ADXL355_Reset_Sensor(void);
#endif /* ADXL355_H_ */


