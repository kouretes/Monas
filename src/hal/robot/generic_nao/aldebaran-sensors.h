/**
 * aldebaran-sensors.h
 *
 *  Created on: Dec 12, 2010
 *      Author: trs
 */

#ifndef ALDEBARAN_SENSORS_H
#define ALDEBARAN_SENSORS_H


#ifdef NAOQI_1_12_5
#include <boost/shared_ptr.hpp>
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
