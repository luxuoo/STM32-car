
#ifndef __PID_H__
#define __PID_H__

#include<stdio.h>
#include<string.h>

typedef struct _PID
{
    double actualValue;     //实际值 每次反馈回来的值，模拟传感器采集到的值 
	double setValue;        //设定值 你期望系统达到的值
    double Kd, Ki, Kp;      //比例、积分、微分常数
	
	
    double P, I, D;         //比例项 积分项 微分项
	double error;           //本次误差值
    double errorPre;        //E[k-1]  //上一次误差
    double errorPrePre;     //E[k-2]  //上上次误差
    double integral;        //积分值
}PID;

/*pid初始化*/
void pid_init(double kp, double ki, double kd, PID *pid);


/*位置式pid
 @param pid  要计算的pid结构体指针
 返回 PID控制器输出值，在模拟仿真里面也作为传感器的值
*/
double pid_position(PID *pid);


/*增量式PID算法
 @param pid  要计算的pid结构体指针
 返回给执行器的输出值
*/
double pid_incremental(PID *pid);

#endif


