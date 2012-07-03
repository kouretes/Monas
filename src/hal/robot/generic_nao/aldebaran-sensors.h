/**
 * aldebaran-sensors.h
 *
 *  Created on: Dec 12, 2010
 *      Author: trs
 */

#ifndef ALDEBARAN_SENSORS_H_
#define ALDEBARAN_SENSORS_H_

#ifdef NAOQI_1_10_10

#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/almotionproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alproxies/dcmproxy.h>
#include <rttools/rttime.h>

#include <boost/bind.hpp>
#define KALBIND boost::bind

#endif

#endif /* ALDEBARAN_SENSORS_H_ */
