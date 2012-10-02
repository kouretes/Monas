/**
* \file Platform/linux/NaoCamera.h
* Interface to the Nao camera using linux-uvc.
* \author Colin Graf
*/

#ifndef NAOCAMERA_H
#define NAOCAMERA_H

#include <linux/videodev2.h>

#ifndef V4L2_CID_AUTOEXPOSURE
#  define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif

#ifndef V4L2_CID_SAT_AUTO
#  define V4L2_CID_SAT_AUTO  (V4L2_CID_BASE+36)
#endif

#ifndef V4L2_CID_EXPOSURE_CORRECTION
#  define V4L2_CID_EXPOSURE_CORRECTION  (V4L2_CID_BASE+34)
#endif

#ifndef V4L2_CID_BACKLIGHT_COMPENSATION
#  define V4L2_CID_BACKLIGHT_COMPENSATION  (V4L2_CID_BASE+28)
#endif

#ifndef V4L2_CID_SHARPNESS
#  define V4L2_CID_SHARPNESS  (V4L2_CID_BASE+27)
#endif

#ifndef V4L2_CID_AWB_G_CHANNEL_GAIN
#  define V4L2_CID_AWB_G_CHANNEL_GAIN (V4L2_CID_BASE+38)
#endif

#ifndef V4L2_CID_UVSAT_RESULT
#  define V4L2_CID_UVSAT_RESULT (V4L2_CID_BASE+39)
#endif

#ifndef V4L2_CID_EDGE_ENH_FACTOR
#  define V4L2_CID_EDGE_ENH_FACTOR (V4L2_CID_BASE+40)
#endif

#ifndef V4L2_CID_DENOISE_STRENGTH
#	define V4L2_CID_DENOISE_STRENGTH (V4L2_CID_BASE+41)
#endif

#ifndef V4L2_CID_AUTO_CONTRAST_CENTER
#  define V4L2_CID_AUTO_CONTRAST_CENTER (V4L2_CID_BASE+42)
#endif

#ifndef V4L2_CID_CONTRAST_CENTER
#  define V4L2_CID_CONTRAST_CENTER (V4L2_CID_BASE+43)
#endif


#ifndef V4L2_CID_AUTOEXPOSURE
#  define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif

#ifndef V4L2_CID_CAM_INIT
#  define V4L2_CID_CAM_INIT         (V4L2_CID_BASE+33)
#endif

#ifndef V4L2_CID_AUDIO_MUTE
#  define V4L2_CID_AUDIO_MUTE       (V4L2_CID_BASE+9)
#endif

#ifndef V4L2_CID_POWER_LINE_FREQUENCY
#  define V4L2_CID_POWER_LINE_FREQUENCY  (V4L2_CID_BASE+24)
enum v4l2_power_line_frequency
{
  V4L2_CID_POWER_LINE_FREQUENCY_DISABLED  = 0,
  V4L2_CID_POWER_LINE_FREQUENCY_50HZ  = 1,
  V4L2_CID_POWER_LINE_FREQUENCY_60HZ  = 2,
};


#define V4L2_CID_HUE_AUTO      (V4L2_CID_BASE+25)
#define V4L2_CID_WHITE_BALANCE_TEMPERATURE  (V4L2_CID_BASE+26)
#define V4L2_CID_SHARPNESS      (V4L2_CID_BASE+27)
#define V4L2_CID_BACKLIGHT_COMPENSATION   (V4L2_CID_BASE+28)

#define V4L2_CID_CAMERA_CLASS_BASE     (V4L2_CTRL_CLASS_CAMERA | 0x900)
#define V4L2_CID_CAMERA_CLASS       (V4L2_CTRL_CLASS_CAMERA | 1)

#define V4L2_CID_EXPOSURE_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+1)

enum  v4l2_exposure_auto_type
{
  V4L2_EXPOSURE_MANUAL = 0,
  V4L2_EXPOSURE_AUTO = 1,
  V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
  V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};
#define V4L2_CID_EXPOSURE_ABSOLUTE    (V4L2_CID_CAMERA_CLASS_BASE+2)
#define V4L2_CID_EXPOSURE_AUTO_PRIORITY    (V4L2_CID_CAMERA_CLASS_BASE+3)

#define V4L2_CID_PAN_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+4)
#define V4L2_CID_TILT_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+5)
#define V4L2_CID_PAN_RESET      (V4L2_CID_CAMERA_CLASS_BASE+6)
#define V4L2_CID_TILT_RESET      (V4L2_CID_CAMERA_CLASS_BASE+7)

#define V4L2_CID_PAN_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+8)
#define V4L2_CID_TILT_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+9)

#define V4L2_CID_FOCUS_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+10)
#define V4L2_CID_FOCUS_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+11)
#define V4L2_CID_FOCUS_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+12)

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */
#define NAO_LOWER_CAMERA 0x02
#define NAO_UPPER_CAMERA 0x01

#include <boost/date_time/posix_time/posix_time.hpp>


//#define USE_USERPTR
/**
* \class NaoCamera
* Interface class to the Nao camera.
*/
class NaoCamera
{
public:

  /** Constructor. */
  NaoCamera();

  /** Destructor. */
  ~NaoCamera();

  /** Verify Nao version. Returns true if the Nao version is at least 3. */
  bool verifyNaoVersion();

  /**
  * The method blocks till a new image arrives.
  * \return true (except a not manageable exception occurs)
  */
  bool captureNew();

  /**
  * The last captured image.
  * \return The image data buffer.
  */
  const unsigned char* getImage() const;

  /**
  * Timestamp of the last captured image.
  * \return The timestamp.
  */
  boost::posix_time::ptime getTimeStamp() const;

  unsigned char  switchToUpper();
  unsigned char switchToLower();
  /**
   * Switches the current camera.
   *
   * camera: The camera to switch to.
   */
  unsigned char switchCamera(unsigned char camera);

  unsigned char getCurrentCamera() { return currentCamera; }

  /**
   * Asserts that the actual camera settings are correct.
   */
  void assertCameraSettings();

  /**
   * Unconditional write of the camera settings
   */
  void writeCameraSettings();

  enum
  {
    frameBufferCount = 3, /**< Amount of available frame buffers. */
    WIDTH = 640,
    HEIGHT = 480,
    SIZE = WIDTH * HEIGHT * 2
  };
    /**
  * Requests the value of a camera control setting from camera.
  * \param id The setting id.
  * \return The value.
  */
  int getControlSetting(unsigned int id);

  /**
  * Sets the value of a camera control setting to camera.
  * \param id The setting id.
  * \param value The value.
  * \return True on success.
  */
  bool setControlSetting(unsigned int id, int value);
private:
  static NaoCamera* theInstance; /**< The only instance of this class. */


  unsigned char currentCamera; /**< The current camera in use */

  int fd; /**< The file descriptor for the video device. */
  void* mem[frameBufferCount]; /**< Frame buffer addresses. */
  int memLength[frameBufferCount]; /**< The length of each frame buffer. */
  struct v4l2_buffer* buf; /**< Reusable parameter struct for some ioctl calls. */
  struct v4l2_buffer* currentBuf; /**< The last dequeued frame buffer. */
  boost::posix_time::ptime timeStamp; /**< Timestamp of the last captured image. */




  /** Open and connects to the i2c adapter device. */
  int openI2CAdapter();

  /** Closes the I2C adapter referenced by the file descriptor 'filedes' */
  void closeI2CAdapter(int filedes);

  void initSelectCamera(  unsigned char camera);
  void initOpenVideoDevice();
  void initSetCameraDefaults();
  void initSetImageFormat();
  void initSetFrameRate();
  void initRequestAndMapBuffers();
  void initQueueAllBuffers();
  void initDefaultControlSettings();
  void initResetCrop();
  void startCapturing();
};

#endif
