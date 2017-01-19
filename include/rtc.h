/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#ifndef RTC_H_
#define RTC_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup rtc_driver Real Time Clock (RTC) or Warl Clock driver.
 * \ingroup HAL
 * \code
 * #include <rtc.h>
 * \endcode
 * \{
 */
/**
 * Handle of a Instants of RTC
 * 
 * The data of this Handle is private and only the driver has access to the data. 
 */
struct rtc;
#ifdef CONFIG_RTC_MULTI
/**
 * Function of a driver in Multi Driver implementation mode 
 */
struct rtc_ops {
};
#endif
/**
 * All driver shall implement this Struct
 * 
 * For RTC: 
 * \code{.c}
 * struct rtc {
 * 	struct rtc_generic gen;
 * }
 * \endcode
 */
struct rtc_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_RTC_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_RTC_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct rtc_ops *ops;
#endif
};
#ifdef CONFIG_NEWLIB
#include <time.h>
#include <sys/time.h>
#else
#include <stdint.h>
typedef int64_t time_t;
/**
 * Timeval
 */
struct timespec {
	/**
	 * seconds
	 */
	time_t tv_sec;
	/**
	 * Nanoseconds
	 */
	int64_t tv_nsec;
};
#endif
#ifndef CONFIG_RTC_MULTI
/**
 * Init Function
 * \param index in rtcs Array
 * \return RTC Instants or NULL
 */
struct rtc *rtc_init(uint32_t index);
/**
 * Deinit Driver Instants 
 * \param rtc Instant
 * \return 0 on ok -1 on failure
 */
int32_t rtc_deinit(struct rtc *rtc);
int32_t rtc_adjust(struct rtc *rtc, struct timespec *offset, TickType_t waittime);
int32_t rtc_getTime(struct rtc *rtc, struct timespec *time, TickType_t waittime);
int32_t rtc_setTime(struct rtc *rtc, struct timespec *time, TickType_t waittime);
int32_t rtc_adjustISR(struct rtc *rtc, struct timespec *offset);
int32_t rtc_getTimeISR(struct rtc *rtc, struct timespec *time);
int32_t rtc_setTimeISR(struct rtc *rtc, struct timespec *time);
#else
inline struct rtc *rtc_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(rtc);
	struct rtc_generic *p = (struct rtc_generic *) HAL_GET_DEV(rtc, index);
	if (p == NULL) {
		return NULL;
	}
	return p->ops->rtc_init(index);
}
inline int32_t rtc_deinit(struct rtc *rtc) {
	struct rtc_generic *p = (struct rtc_generic *) rtc;
	return p->ops->rtc_deinit(rtc);
}
#endif
/**\}*/
#endif
