#include "ADC_App.h"
#include "stdio.h"
#include "main.h"

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
uint16_t Get_Adc(uint32_t ch)   
{
//    ADC_ChannelConfTypeDef ADC1_ChanConf;
//    
//    ADC1_ChanConf.Channel=ch;                                   //通道
//    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
//    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_160CYCLES_5;      //采样时间                
//    HAL_ADC_ConfigChannel(&BAT_HADC,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&BAT_HADC);                               //开启ADC
	
    HAL_ADC_PollForConversion(&BAT_HADC,10);                //轮询转换
 
	return (uint16_t)HAL_ADC_GetValue(&BAT_HADC);	        //返回最近一次ADC1规则组的转换结果
}

#define V_NUM		10
float Read_Bat_Voltage(void)
{
	uint8_t i;
	uint16_t vol[V_NUM];
	uint16_t	max,min;
	float Voltage=0;
	BAT_CHECK_ENABLE();		//打开电源
	HAL_Delay(15);					//15MS
	for(i=0;i<V_NUM;i++)
	{
		HAL_ADC_Start(&BAT_HADC);   ////开启ADC
		HAL_ADC_PollForConversion(&BAT_HADC,10);            //轮询转换
		vol[i]=(uint16_t)HAL_ADC_GetValue(&BAT_HADC);	      //返回最近一次ADC1规则组的转换结果	
	}
	BAT_CHECK_DISABLE();	//关闭电源
	max=min=vol[0];
	
	for(i=0;i<V_NUM;i++)
	{
		Voltage += vol[i];
		if(vol[i]<min) min = vol[i];	//最小值
		if(vol[i]>max) max = vol[i];	//最大值
	}
	Voltage = Voltage-min-max;
	Voltage = Voltage/(V_NUM-2);
	Voltage	= Voltage*3.3/2048;		// Voltage = Voltage*3.3/4096 *2 
	
	//APP_LOG("Bat:%.2fV\r\n\r\n",Voltage);
	return Voltage;		// Voltage = Voltage*3.3/4096 *2 
}

