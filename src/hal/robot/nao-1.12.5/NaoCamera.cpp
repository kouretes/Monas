/**
* \file Platform/linux/NaoCamera.cpp
* Interface to the Nao camera using linux-uvc.
* \author Colin Graf
* \author Thomas Röfer
*/
#ifndef KROBOT_IS_REMOTE

#ifdef NDEBUG
#undef NDEBUG
#endif
#include "hal/robot/generic_nao/NaoCamera.h"

#include "core/include/Logger.hpp"

#include <assert.h>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#ifdef USE_USERPTR
#include <malloc.h> // memalign
#endif



#undef __STRICT_ANSI__
#include <bn/i2c/i2c-dev.h>
//#include <linux/i2c.h>

#include <linux/version.h>
//#include <linux/i2c-dev.h>
#define __STRICT_ANSI__



NaoCamera *NaoCamera::theInstance = 0;

NaoCamera::NaoCamera(userPrefs newPrefs) :
	currentCamera (NAO_LOWER_CAMERA), //Lower
	currentBuf (0),
	timeStamp()
{
	assert (theInstance == 0);
	theInstance = this;
	uPreferences = newPrefs;
	unsigned char current = currentCamera;
	unsigned char other = currentCamera == NAO_UPPER_CAMERA ?  NAO_LOWER_CAMERA : NAO_UPPER_CAMERA;
	assert (verifyNaoVersion() == true);
	LogEntry(LogLevel::ExtraInfo,"NaoCamera")<< "Initializing Upper Camera";
	initSelectCamera (NAO_UPPER_CAMERA);
	initOpenVideoDevice();
	initSetCameraDefaults();
	LogEntry(LogLevel::ExtraInfo,"NaoCamera")<< "Initializing Lower Camera";
	initSelectCamera (NAO_LOWER_CAMERA);
	initSetCameraDefaults();
	initSelectCamera (other);
	initRequestAndMapBuffers();
	initQueueAllBuffers();
	initResetCrop();
	initSetImageFormat();
	initSetFrameRate();
	initDefaultControlSettings();
	initSelectCamera (current);
	initResetCrop();
	initSetImageFormat();
	initSetFrameRate();
	initDefaultControlSettings();
	startCapturing();
}

NaoCamera::~NaoCamera()
{
	// disable streaming
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	assert (ioctl (fd, VIDIOC_STREAMOFF, &type) != -1);

	// unmap buffers
	for (int i = 0; i < frameBufferCount; ++i)
#ifdef USE_USERPTR
		free (mem[i]);

#else
		munmap (mem[i], memLength[i]);
#endif
	// close the device
	close (fd);
	free (buf);
	theInstance = 0;
}

bool NaoCamera::captureNew()
{
	// requeue the buffer of the last captured image which is obsolete now
	if (currentBuf)
	{
		assert (ioctl (fd, VIDIOC_QBUF, currentBuf) != -1);
		currentBuf=NULL;
	}

	struct pollfd pollfd = {fd, POLLIN | POLLPRI, 0};

	int polled = poll (&pollfd, 1, 5); // Fail after missing 30 frames (1.0s)

	if (polled < 0)
	{
		LogEntry(LogLevel::FatalError,"NaoCamera") <<  "Cannot poll. Reason: " << strerror (errno);
		assert (false);
	}
	else if (polled == 0)
	{
		return false;
	}
	else if (pollfd.revents & (POLLERR | POLLNVAL) )
	{
		LogEntry(LogLevel::FatalError,"NaoCamera") <<  "Polling failed.";
		assert (false);
	}

	// dequeue a frame buffer (this call blocks when there is no new image available) */
	assert (ioctl (fd, VIDIOC_DQBUF, buf) != -1);
	timeStamp = boost::posix_time::microsec_clock::universal_time();
	assert (buf->bytesused == SIZE);
	currentBuf = buf;
	static bool shout = true;

	if (shout)
	{
		shout = false;
		LogEntry(LogLevel::ExtraInfo,"NaoCamera") <<  "Camera is working " << (timeStamp);
	}

	return true;
}

const unsigned char *NaoCamera::getImage() const
{
#ifdef USE_USERPTR
	std::cout << currentBuf << std::endl;
	return currentBuf ? (unsigned char *) currentBuf->m.userptr : 0;
#else
	return currentBuf ? static_cast<unsigned char *> (mem[currentBuf->index]) : 0;
#endif
}

boost::posix_time::ptime NaoCamera::getTimeStamp() const
{
	assert (currentBuf);
	return timeStamp;
}

int NaoCamera::getControlSetting (unsigned int id)
{
	struct v4l2_queryctrl queryctrl;
	queryctrl.id = id;

	if (ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
	{
		return -1;
	}

	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		return -1;    // not available
	}

	if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
	{
		return -1;    // not supported
	}

	struct v4l2_control control_s;

	control_s.id = id;

	if (ioctl (fd, VIDIOC_G_CTRL, &control_s) < 0)
	{
		return -1;
	}

	if (control_s.value == queryctrl.default_value)
	{
		return -1;
	}

	return control_s.value;
}



bool NaoCamera::setControlSetting (unsigned int id, int value)
{
	struct v4l2_queryctrl queryctrl;
	queryctrl.id = id;

	if (ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
	{
		return false;
	}

	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		return false;    // not available
	}

	if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
	{
		return false;    // not supported
	}

	// clip value
	if (value < queryctrl.minimum)
	{
		value = queryctrl.minimum;
	}

	if (value > queryctrl.maximum)
	{
		value = queryctrl.maximum;
	}

	if (value < 0)
	{
		value = queryctrl.default_value;
	}

	struct v4l2_control control_s;

	control_s.id = id;

	control_s.value = value;

	if (ioctl (fd, VIDIOC_S_CTRL, &control_s) < 0)
	{
		return false;
	}

	return true;
}


unsigned char NaoCamera::switchCamera (  unsigned char camera)
{
	unsigned char cmd[2] =  {camera, 0};
	int flip = camera == NAO_UPPER_CAMERA ? 1 : 0;
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// disable streaming
	assert (ioctl (fd, VIDIOC_STREAMOFF, &type) != -1);
	// switch camera
	int i2cfd = openI2CAdapter();
	bool success = false;
	for(int i=0;i<10;i++){
		if(i2c_smbus_write_block_data (i2cfd, 220, 1, cmd) != -1){
			success = true;
			break;
		}
	}
	assert (success);

	closeI2CAdapter (i2cfd);
	setControlSetting (V4L2_CID_VFLIP, flip);
	setControlSetting (V4L2_CID_HFLIP, flip);
	// enable streaming
	assert (ioctl (fd, VIDIOC_STREAMON, &type) != -1);
	currentCamera = camera;
	return camera;
}

unsigned char NaoCamera::switchToUpper()
{
	if (currentCamera == NAO_UPPER_CAMERA)
	{
		return NAO_UPPER_CAMERA;
	}

	return switchCamera (NAO_UPPER_CAMERA);
}

unsigned char NaoCamera::switchToLower()
{
	if (currentCamera == NAO_LOWER_CAMERA)
	{
		return NAO_LOWER_CAMERA;
	}

	return switchCamera (NAO_LOWER_CAMERA);
}

void NaoCamera::initSelectCamera (  unsigned char camera)
{
	unsigned char cmd[2] = {camera, 0};
	int i2cfd = openI2CAdapter();
	assert (i2c_smbus_write_block_data (i2cfd, 220, 1, cmd) != -1); // select camera
	LogEntry(LogLevel::ExtraInfo,"NaoCamera") <<  "Going to Cam: " << ( (int) camera);
	closeI2CAdapter (i2cfd);
	currentCamera = camera;
}

void NaoCamera::initOpenVideoDevice()
{
	// open device
	fd = open ("/dev/video0", O_RDWR);
	assert (fd != -1);
}

void NaoCamera::initSetCameraDefaults()
{
	// set default parameters
	struct v4l2_control control;
	memset (&control, 0, sizeof (control) );
	control.id = V4L2_CID_CAM_INIT;
	control.value = 0;
	assert (ioctl (fd, VIDIOC_S_CTRL, &control) >= 0);
	v4l2_std_id esid0 = WIDTH == 320 ? 0x04000000UL : 0x08000000UL;
	assert (!ioctl (fd, VIDIOC_S_STD, &esid0) );
}

void NaoCamera::initSetImageFormat()
{
	// set format
	struct v4l2_format fmt;
	memset (&fmt, 0, sizeof (struct v4l2_format) );
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = WIDTH;
	fmt.fmt.pix.height = HEIGHT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	assert (!ioctl (fd, VIDIOC_S_FMT, &fmt) );
	assert (fmt.fmt.pix.sizeimage == SIZE);
}

void NaoCamera::initSetFrameRate()
{
	// set frame rate
	struct v4l2_streamparm fps;
	memset (&fps, 0, sizeof (struct v4l2_streamparm) );
	fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	assert (!ioctl (fd, VIDIOC_G_PARM, &fps) );
	fps.parm.capture.timeperframe.numerator = 1;
	fps.parm.capture.timeperframe.denominator = 30;
	assert (ioctl (fd, VIDIOC_S_PARM, &fps) != -1);
}

void NaoCamera::initRequestAndMapBuffers()
{
	// request buffers
	struct v4l2_requestbuffers rb;
	memset (&rb, 0, sizeof (struct v4l2_requestbuffers) );
	rb.count = frameBufferCount;
	rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef USE_USERPTR
	rb.memory = V4L2_MEMORY_USERPTR;
#else
	rb.memory = V4L2_MEMORY_MMAP;
#endif
	assert (ioctl (fd, VIDIOC_REQBUFS, &rb) != -1);
	assert (rb.count == frameBufferCount);
	// map or prepare the buffers
	buf = static_cast<struct v4l2_buffer *> (calloc (1, sizeof (struct v4l2_buffer) ) );
#ifdef USE_USERPTR
	unsigned int bufferSize = SIZE;
	unsigned int pageSize = getpagesize();
	bufferSize = (bufferSize + pageSize - 1) & ~ (pageSize - 1);
#endif

	for (int i = 0; i < frameBufferCount; ++i)
	{
#ifdef USE_USERPTR
		memLength[i] = bufferSize;
		mem[i] = memalign (pageSize, bufferSize);
#else
		buf->index = i;
		buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf->memory = V4L2_MEMORY_MMAP;
		assert (ioctl (fd, VIDIOC_QUERYBUF, buf) != -1);
		memLength[i] = buf->length;
		mem[i] = mmap (0, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf->m.offset);
		assert (mem[i] != MAP_FAILED);
#endif
	}
}

void NaoCamera::initQueueAllBuffers()
{
	// queue the buffers
	for (int i = 0; i < frameBufferCount; ++i)
	{
		buf->index = i;
		buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef USE_USERPTR
		buf->memory = V4L2_MEMORY_USERPTR;
		buf->m.userptr = (unsigned long) mem[i];
		buf->length  = memLength[i];
#else
		buf->memory = V4L2_MEMORY_MMAP;
#endif
		assert (ioctl (fd, VIDIOC_QBUF, buf) != -1);
	}
}

void NaoCamera::initDefaultControlSettings()
{
	// make sure automatic stuff is off
	int flip = currentCamera == NAO_UPPER_CAMERA ? 1 : 0;
	setControlSetting (V4L2_CID_HFLIP, flip);
	setControlSetting (V4L2_CID_VFLIP, flip);
	//std::list<CameraSettings::V4L2Setting> v4l2settings = settings.getInitSettings();
	//assert(setControlSettings(v4l2settings));
	setControlSetting (V4L2_CID_AUTOEXPOSURE , 0);
	setControlSetting (V4L2_CID_AUTO_WHITE_BALANCE, 0);
	setControlSetting (V4L2_CID_AUTOGAIN, 0);
	setControlSetting (V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
	setControlSetting (V4L2_CID_BACKLIGHT_COMPENSATION, 0);
	setControlSetting (V4L2_CID_SAT_AUTO, 0);
	setControlSetting (V4L2_CID_HUE_AUTO, 0);
	setControlSetting (V4L2_CID_AUTO_CONTRAST_CENTER, 0);
	setControlSetting (V4L2_CID_POWER_LINE_FREQUENCY, V4L2_CID_POWER_LINE_FREQUENCY_DISABLED);
	setControlSetting (V4L2_CID_HUE, 0);
	setControlSetting (V4L2_CID_SATURATION, 255);
	setControlSetting (V4L2_CID_BRIGHTNESS, 128);

	setControlSetting (V4L2_CID_AWB_G_CHANNEL_GAIN, uPreferences.GREEN_GAIN);
	setControlSetting (V4L2_CID_CONTRAST, uPreferences.CONTRAST);
	setControlSetting (V4L2_CID_RED_BALANCE, uPreferences.RED_BALANCE);
	setControlSetting (V4L2_CID_BLUE_BALANCE, uPreferences.BLUE_BALANCE);
	setControlSetting (V4L2_CID_GAIN, uPreferences.GAIN);
	usleep (300 * 1000);
}

bool NaoCamera::setUserSettings(userPrefs newPrefs){
	bool success = true;
	if(newPrefs.GREEN_GAIN != uPreferences.GREEN_GAIN){
		uPreferences.GREEN_GAIN = newPrefs.GREEN_GAIN;
		success &= setControlSetting (V4L2_CID_AWB_G_CHANNEL_GAIN, uPreferences.GREEN_GAIN);
	}
	if(newPrefs.CONTRAST != uPreferences.CONTRAST){
		uPreferences.CONTRAST = newPrefs.CONTRAST;
		success &= setControlSetting (V4L2_CID_CONTRAST, uPreferences.CONTRAST);
	}
	if(newPrefs.RED_BALANCE != uPreferences.RED_BALANCE){
		uPreferences.RED_BALANCE = newPrefs.RED_BALANCE;
		success &= setControlSetting (V4L2_CID_RED_BALANCE, uPreferences.RED_BALANCE);
	}
	if(newPrefs.BLUE_BALANCE != uPreferences.BLUE_BALANCE){
		uPreferences.BLUE_BALANCE = newPrefs.BLUE_BALANCE;
		success &= setControlSetting (V4L2_CID_BLUE_BALANCE, uPreferences.BLUE_BALANCE);
	}
	if(newPrefs.GAIN != uPreferences.GAIN){
		uPreferences.GAIN = newPrefs.GAIN;
		success &= setControlSetting (V4L2_CID_GAIN, uPreferences.GAIN);
	}
	return success;
}

void NaoCamera::initResetCrop()
{
	struct v4l2_cropcap cropcap;
	memset (&cropcap, 0, sizeof (cropcap) );
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	assert (ioctl (fd, VIDIOC_CROPCAP, &cropcap) != -1);
	struct v4l2_crop crop;
	memset (&crop, 0, sizeof (crop) );
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c = cropcap.defrect;
	/* errno will be set to EINVAL if cropping is unsupported,
	 * which would be fine, too */
	assert (ioctl (fd, VIDIOC_S_CROP, &crop) != -1 || errno == EINVAL);
}

void NaoCamera::startCapturing()
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	assert (ioctl (fd, VIDIOC_STREAMON, &type) != -1);
}

void NaoCamera::assertCameraSettings()
{
	bool allFine = true;
	// check frame rate
	struct v4l2_streamparm fps;
	memset (&fps, 0, sizeof (fps) );
	fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	assert (!ioctl (fd, VIDIOC_G_PARM, &fps) );

	if (fps.parm.capture.timeperframe.numerator != 1)
	{
		//OUTPUT(idText, text, "fps.parm.capture.timeperframe.numerator is wrong.");
		allFine = false;
	}

	if (fps.parm.capture.timeperframe.denominator != 30)
	{
		//OUTPUT(idText, text, "fps.parm.capture.timeperframe.denominator is wrong.");
		allFine = false;
	}
}


int NaoCamera::openI2CAdapter()
{
	int i2cfd = open ("/dev/i2c-0", O_RDWR);
	assert (i2cfd != -1);
	assert (ioctl (i2cfd, 0x703, 8) == 0);
	return i2cfd;
}

void NaoCamera::closeI2CAdapter (int filedes)
{
	close (filedes);
}

bool NaoCamera::verifyNaoVersion()
{
	int i2cfd = openI2CAdapter();
	bool versionok = i2c_smbus_read_byte_data (i2cfd, 170) >= 2;
	closeI2CAdapter (i2cfd);
	return versionok;
}

#endif
