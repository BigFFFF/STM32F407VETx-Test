/*
 * filter.h
 *
 *  Created on: Sep 5, 2022
 *      Author: A
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define DATA_LEN	360				//数据长度
#define V_MAX		4000			//电压最大值
#define V_MIN		0				//电压最小值
#define V_SINGE		((V_MAX)/(100))	//电压最小分度值
#define A_MAX		3000			//电流最大值
#define A_MIN		0				//电压最小值
#define A_SINGE		((V_MAX)/(100))	//电流最小分度值

typedef struct {
	int adc_x;		//新进的值
	int first;		//最早的值
	int old;		//前一个值
	int sum;		//累加值
	int differe;	//新旧差值
}Filter_ch, * pFilter_ch;

typedef struct {
	int adc_x;		//新进的值
	int old;		//前一个值
	div_t output;	//除数值(余和商)
}Filter_led, * pFilter_led;

extern Filter_ch ch3;
extern Filter_led led3;

/***************************
ADC一次滤波(limit_x)
参数：
pFilter_ch ch		//单个通道保存电压值的结构体指针
typedef struct {
	int adc_x;		//新进的值
	int first;		//最早的值
	int old;		//前一个值
	int sum;		//累加值
	int differe;	//新旧差值
}Filter_ch, * pFilter_ch;

int limit_x 		//单个LED显示的电压值分度值

返回值：
int					//单个电压/电流值（V_MIN-V_MAX）/（A_MIN-A_MAX）
***************************/
int filter_x(pFilter_ch ch, int limit_x);

/***************************
作用：
ADC二次隔值滤波(limit_x)

参数：
pFilter_led led		//单个通道保存电压值的结构体指针
typedef struct {
	int adc_x;		//新进的值
	int old;		//前一个值
	div_t output;	//除数值(余和商)
}Filter_led, * pFilter_led;

int limit_x 		//单个LED显示的电压值分度值

返回值：
int 				//LED个数(0-100）
***************************/
int filter_led(pFilter_led led, int limit_x);

#ifdef __cplusplus
}
#endif

#endif /* INC_FILTER_H_ */
