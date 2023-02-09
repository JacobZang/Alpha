#include "bsp_rtc.h"


/* RTC初始化 */
void rtc_init(void)
{
    SNVS->HPCOMR |= (1 << 31) | (1 << 8);

    /* 开启RTC */
    rtc_enable();
}

/* 使能RTC */
void rtc_enable(void)
{
    SNVS->LPCR |= (1 << 0);
    while ((SNVS->LPCR & 0x01) == 0);
}

/* 关闭RTC */
void rtc_disable(void)
{
    SNVS->LPCR &= ~(1 << 0);
    while ((SNVS->LPCR & 0x01) == 1);
}

/*
 * @description		: 将时间转换为秒数
 * @param - datetime: 要转换日期和时间。
 * @return 			: 转换后的秒数
 */
unsigned int rtc_coverdate_to_seconds(struct rtc_datetime *datetime)
{	
	unsigned short i = 0;
	unsigned int seconds = 0;
	unsigned int days = 0;
	unsigned short monthdays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
	
	for(i = 1970; i < datetime->year; i++)
	{
		days += DAYS_IN_YEAR; 		/* 平年，每年365天 */
		if(rtc_isleapyear(i)) days += 1;/* 闰年多加一天 		*/
	}

	days += monthdays[datetime->month];
	if(rtc_isleapyear(i) && (datetime->month >= 3)) days += 1;/* 闰年，并且当前月份大于等于3月的话加一天 */

	days += datetime->day - 1;

	seconds = days * SECONDS_IN_DAY + 
				datetime->hour * SECONDS_IN_HOUR +
				datetime->minute * SECONDS_IN_MINUTE +
				datetime->second;

	return seconds;	
}

/* 时间设置函数 */
void rtc_setdatetime(struct rtc_datetime *datetime)
{
    unsigned int seconds = 0;
    unsigned int tmp = SNVS->LPCR;

    rtc_enable();

    seconds = rtc_coverdate_to_seconds(datetime);

    SNVS->LPSRTCMR = (unsigned int)(seconds >> 17); /* 高16位 */
    SNVS->LPSRTCLR = (unsigned int)(seconds << 15); /* 低16位 */

    if (tmp & 0x01) /* 以前已经打开了Rtc */
        rtc_enable();
}

/* 读取秒数 */
unsigned int rtc_getseconds(void)
{
    unsigned int seconds = 0;
    seconds = (SNVS->LPSRTCLR >> 15) | (SNVS->LPSRTCMR << 17);
    return seconds;
}

void rtc_convertseconds_to_datetime(u64 seconds, struct rtc_datetime *datetime)
{
    u64 x;
    u64  secondsRemaining, days;
    unsigned short daysInYear;

    /* 每个月的天数       */
    unsigned char daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    secondsRemaining = seconds; /* 剩余秒数初始化 */
    days = secondsRemaining / SECONDS_IN_DAY + 1; 		/* 根据秒数计算天数,加1是当前天数 */
    secondsRemaining = secondsRemaining % SECONDS_IN_DAY; /*计算天数以后剩余的秒数 */

	/* 计算时、分、秒 */
    datetime->hour = secondsRemaining / SECONDS_IN_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_HOUR;
    datetime->minute = secondsRemaining / 60;
    datetime->second = secondsRemaining % SECONDS_IN_MINUTE;

    /* 计算年 */
    daysInYear = DAYS_IN_YEAR;
    datetime->year = YEAR_RANGE_START;
    while(days > daysInYear)
    {
        /* 根据天数计算年 */
        days -= daysInYear;
        datetime->year++;

        /* 处理闰年 */
        if (!rtc_isleapyear(datetime->year))
            daysInYear = DAYS_IN_YEAR;
        else	/*闰年，天数加一 */
            daysInYear = DAYS_IN_YEAR + 1;
    }
	/*根据剩余的天数计算月份 */
    if(rtc_isleapyear(datetime->year)) /* 如果是闰年的话2月加一天 */
        daysPerMonth[2] = 29;

    for(x = 1; x <= 12; x++)
    {
        if (days <= daysPerMonth[x])
        {
            datetime->month = x;
            break;
        }
        else
        {
            days -= daysPerMonth[x];
        }
    }
    datetime->day = days;
}

/*
 * @description	: 判断指定年份是否为闰年，闰年条件如下:
 * @param - year: 要判断的年份
 * @return 		: 1 是闰年，0 不是闰年
 */
unsigned char rtc_isleapyear(unsigned short year)
{	
	unsigned char value=0;
	
	if(year % 400 == 0)
		value = 1;
	else 
	{
		if((year % 4 == 0) && (year % 100 != 0))
			value = 1;
		else 
			value = 0;
	}
	return value;
}


/* 获取时间 */
void rtc_getdatetime(struct rtc_datetime *datetime)
{
    unsigned int seconds = 0;
    seconds = rtc_getseconds();
    rtc_convertseconds_to_datetime(seconds, datetime);
}