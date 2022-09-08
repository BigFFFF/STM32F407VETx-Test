/*
 * filter.c
 *
 *  Created on: Sep 5, 2022
 *      Author: A
 */
#include "filter.h"

Filter_ch ch[CH_MAX] = {{ 0 }};
Filter_led led[CH_MAX] = {{ 0 }};

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
	int count;		//计数校准
}Filter_ch, * pFilter_ch;

int limit_x 		//单个LED显示的电压值分度值

返回值：
int					//单个电压/电流值（V_MIN-V_MAX）/（A_MIN-A_MAX）
***************************/
int filter_x(pFilter_ch ch, int limit_x) {
	ch->differe = ch->adc_x - ch->old;
	ch->old = ch->adc_x;

	//差值超过limit_x的允许变化
	if (ch->differe > limit_x || ch->differe < -limit_x) {
		ch->first = ch->adc_x;
		ch->sum = 0;
		ch->count = 0;
		return ch->first;
	}

	//累积变化量，累积超过limit_x的一半允许变化
	ch->sum += ch->differe;
	if ((ch->sum > limit_x || ch->sum < -limit_x)) {
		ch->first += ch->sum;
		ch->sum = 0;
		ch->count = 0;
		return ch->first;
	}

	//累积10次不变时,且变化幅度超过limit_x的一半时，滞后平均滤波(根据采集频率而定)
	ch->count++;
	if ((ch->count > 10) && ((ch->adc_x - ch->first) > (limit_x >> 1) || (ch->adc_x - ch->first) < -(limit_x >> 1))) {
		ch->first = (ch->adc_x + ch->first) >> 1;
		ch->sum = 0;
		ch->count = 0;
		return ch->first;
	}

	return ch->first;
}

/***************************
ADC二次隔值滤波(limit_x)

参数：
pFilter_led led		//单个通道保存电压值的结构体指针
typedef struct {
	int adc_x;		//新进的值
	int old;		//前一个LED值
	div_t output;	//除数值(余和商)
}Filter_led, * pFilter_led;

int limit_x 		//单个LED显示的电压值分度值

返回值：
int 				//LED个数(0-100）
***************************/
int filter_led(pFilter_led led, int limit_x) {
	led->output = div(led->adc_x, limit_x);

	//五舍五入
	if (led->output.rem > (limit_x >> 1)) {
		led->old = led->output.quot + 1;
	}
	else if (led->output.rem < (limit_x >> 1)) {
		led->old = led->output.quot;
	}
	return led->old;
}

