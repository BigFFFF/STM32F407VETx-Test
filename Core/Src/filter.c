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
ADC限值滤波(limit_x)
参数：
pFilter_ch ch		//单个通道保存电压值的结构体指针
typedef struct {
	int adc_x;		//新进的值
	int old_x;		//目前的值
	int diff_x;		//变化前差值
}Filter_ch, * pFilter_ch;

int limit_x 		//单个LED显示的电压值分度值

返回值：
int 				//LED个数(0-100）
***************************/
int filter_x(pFilter_ch ch, int limit_x) {
	int difference = ch->adc_x - ch->old_x;

	//变化前差值累加
	ch->diff_x += difference;

	//差值超过limit_x的立刻变化
	if (difference > (limit_x >> 1) || difference < -(limit_x >> 1)){
		ch->old_x = ch->adc_x;
		ch->diff_x = 0;
	}

	return ch->old_x;
}

/***************************
LED四舍五入滤波(limit_x)
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

	//四舍五入
	if (led->output.rem > (limit_x >> 1)) {
		led->old = led->output.quot + 1;
	}
	else {
		led->old = led->output.quot;
	}
	return led->old;
}

