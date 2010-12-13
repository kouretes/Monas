/**
 * aldebaran-motion.h
 *
 *  Created on: Dec 12, 2010
 *      Author: trs
 */

#ifndef ALDEBARAN_MOTION_H_
#define ALDEBARAN_MOTION_H_

#ifdef NAOQI_1_6_0
#include <alptr.h>
#include <alvalue.h>

#include <albroker.h>
#include <alproxy.h>
#include <almotionproxy.h>
#include <alframemanagerproxy.h>

#include <almemoryproxy.h>

#include <altexttospeechproxy.h>

#elif NAOQI_1_8_16
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#include <alvalue/alvalue.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alframemanagerproxy.h>

#endif

#endif /* ALDEBARAN_MOTION_H_ */
