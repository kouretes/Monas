/**
 * aldebaran-sensors.h
 *
 *  Created on: Dec 12, 2010
 *      Author: trs
 */

#ifndef ALDEBARAN_SENSORS_H_
#define ALDEBARAN_SENSORS_H_

#ifdef NAOQI_1_6_0
#include <almotionproxy.h>
#include <almemoryproxy.h>
//#include <almemoryfastaccess.h>
#include <alptr.h>
//#include <alxplatform.h>
//#include <albroker.h>
#include <alproxy.h>
#include <dcmproxy.h>
#include <rttime.h>

#define KALBIND AL::functional::bind

#elif NAOQI_1_8_16
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/dcmproxy.h>
#include <rttools/rttime.h>

#define KALBIND boost::bind
#endif

#endif /* ALDEBARAN_SENSORS_H_ */
