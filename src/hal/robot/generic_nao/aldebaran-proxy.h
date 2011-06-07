/**
 * aldebaran-proxy.h
 *
 *  Created on: Dec 13, 2010
 *      Author: trs
 */

#ifndef ALDEBARAN_PROXY_H_
#define ALDEBARAN_PROXY_H_
#ifdef NAOQI_1_6_0
#include <alptr.h>
#include <alproxy.h>
#include <almemoryproxy.h>

#elif NAOQI_1_8_16
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>

#elif NAOQI_1_10_10
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>

#endif
#endif /* ALDEBARAN_PROXY_H_ */
