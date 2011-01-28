/**
 * aldebaran-vision.h
 *
 *  Created on: Dec 12, 2010
 *      Author: trs
 */

#ifndef ALDEBARAN_VISION_H_
#define ALDEBARAN_VISION_H_
#ifdef NAOQI_1_6_0
#include <alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>
#include <alproxy.h>
#include <alptr.h>
//#include "alxplatform.h"
#elif NAOQI_1_8_16
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#elif NAOQI_1_10_10
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#endif
#endif /* ALDEBARAN_VISION_H_ */
