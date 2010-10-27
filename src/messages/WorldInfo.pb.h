// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: WorldInfo.proto

#ifndef PROTOBUF_WorldInfo_2eproto__INCLUDED
#define PROTOBUF_WorldInfo_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "VisionObservations.pb.h"
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_WorldInfo_2eproto();
void protobuf_AssignDesc_WorldInfo_2eproto();
void protobuf_ShutdownFile_WorldInfo_2eproto();

class WorldInfo;
class RobotPose;
class Ball;
class UnknownObjects;
class LocalizationData;
class header;

// ===================================================================

class WorldInfo : public ::google::protobuf::Message {
 public:
  WorldInfo();
  virtual ~WorldInfo();
  
  WorldInfo(const WorldInfo& from);
  
  inline WorldInfo& operator=(const WorldInfo& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const WorldInfo& default_instance();
  
  void Swap(WorldInfo* other);
  
  // implements Message ----------------------------------------------
  
  WorldInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const WorldInfo& from);
  void MergeFrom(const WorldInfo& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required .RobotPose myPosition = 1;
  inline bool has_myposition() const;
  inline void clear_myposition();
  static const int kMyPositionFieldNumber = 1;
  inline const ::RobotPose& myposition() const;
  inline ::RobotPose* mutable_myposition();
  
  // repeated .Ball Balls = 2;
  inline int balls_size() const;
  inline void clear_balls();
  static const int kBallsFieldNumber = 2;
  inline const ::Ball& balls(int index) const;
  inline ::Ball* mutable_balls(int index);
  inline ::Ball* add_balls();
  inline const ::google::protobuf::RepeatedPtrField< ::Ball >&
      balls() const;
  inline ::google::protobuf::RepeatedPtrField< ::Ball >*
      mutable_balls();
  
  // repeated .RobotPose OtherRobots = 3;
  inline int otherrobots_size() const;
  inline void clear_otherrobots();
  static const int kOtherRobotsFieldNumber = 3;
  inline const ::RobotPose& otherrobots(int index) const;
  inline ::RobotPose* mutable_otherrobots(int index);
  inline ::RobotPose* add_otherrobots();
  inline const ::google::protobuf::RepeatedPtrField< ::RobotPose >&
      otherrobots() const;
  inline ::google::protobuf::RepeatedPtrField< ::RobotPose >*
      mutable_otherrobots();
  
  // repeated .UnknownObjects UFOs = 4;
  inline int ufos_size() const;
  inline void clear_ufos();
  static const int kUFOsFieldNumber = 4;
  inline const ::UnknownObjects& ufos(int index) const;
  inline ::UnknownObjects* mutable_ufos(int index);
  inline ::UnknownObjects* add_ufos();
  inline const ::google::protobuf::RepeatedPtrField< ::UnknownObjects >&
      ufos() const;
  inline ::google::protobuf::RepeatedPtrField< ::UnknownObjects >*
      mutable_ufos();
  
  // @@protoc_insertion_point(class_scope:WorldInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::RobotPose* myposition_;
  ::google::protobuf::RepeatedPtrField< ::Ball > balls_;
  ::google::protobuf::RepeatedPtrField< ::RobotPose > otherrobots_;
  ::google::protobuf::RepeatedPtrField< ::UnknownObjects > ufos_;
  friend void  protobuf_AddDesc_WorldInfo_2eproto();
  friend void protobuf_AssignDesc_WorldInfo_2eproto();
  friend void protobuf_ShutdownFile_WorldInfo_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static WorldInfo* default_instance_;
};
// -------------------------------------------------------------------

class RobotPose : public ::google::protobuf::Message {
 public:
  RobotPose();
  virtual ~RobotPose();
  
  RobotPose(const RobotPose& from);
  
  inline RobotPose& operator=(const RobotPose& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const RobotPose& default_instance();
  
  void Swap(RobotPose* other);
  
  // implements Message ----------------------------------------------
  
  RobotPose* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RobotPose& from);
  void MergeFrom(const RobotPose& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required float X = 1 [default = -100000];
  inline bool has_x() const;
  inline void clear_x();
  static const int kXFieldNumber = 1;
  inline float x() const;
  inline void set_x(float value);
  
  // required float Y = 2 [default = -100000];
  inline bool has_y() const;
  inline void clear_y();
  static const int kYFieldNumber = 2;
  inline float y() const;
  inline void set_y(float value);
  
  // required float phi = 3 [default = -100000];
  inline bool has_phi() const;
  inline void clear_phi();
  static const int kPhiFieldNumber = 3;
  inline float phi() const;
  inline void set_phi(float value);
  
  // required float confidence = 4 [default = -100000];
  inline bool has_confidence() const;
  inline void clear_confidence();
  static const int kConfidenceFieldNumber = 4;
  inline float confidence() const;
  inline void set_confidence(float value);
  
  // @@protoc_insertion_point(class_scope:RobotPose)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  float x_;
  float y_;
  float phi_;
  float confidence_;
  friend void  protobuf_AddDesc_WorldInfo_2eproto();
  friend void protobuf_AssignDesc_WorldInfo_2eproto();
  friend void protobuf_ShutdownFile_WorldInfo_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static RobotPose* default_instance_;
};
// -------------------------------------------------------------------

class Ball : public ::google::protobuf::Message {
 public:
  Ball();
  virtual ~Ball();
  
  Ball(const Ball& from);
  
  inline Ball& operator=(const Ball& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Ball& default_instance();
  
  void Swap(Ball* other);
  
  // implements Message ----------------------------------------------
  
  Ball* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Ball& from);
  void MergeFrom(const Ball& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required float relativeX = 1 [default = -100000];
  inline bool has_relativex() const;
  inline void clear_relativex();
  static const int kRelativeXFieldNumber = 1;
  inline float relativex() const;
  inline void set_relativex(float value);
  
  // required float relativeY = 2 [default = -100000];
  inline bool has_relativey() const;
  inline void clear_relativey();
  static const int kRelativeYFieldNumber = 2;
  inline float relativey() const;
  inline void set_relativey(float value);
  
  // required float relativeXspeed = 3 [default = -100000];
  inline bool has_relativexspeed() const;
  inline void clear_relativexspeed();
  static const int kRelativeXspeedFieldNumber = 3;
  inline float relativexspeed() const;
  inline void set_relativexspeed(float value);
  
  // required float relativeYspeed = 4 [default = -100000];
  inline bool has_relativeyspeed() const;
  inline void clear_relativeyspeed();
  static const int kRelativeYspeedFieldNumber = 4;
  inline float relativeyspeed() const;
  inline void set_relativeyspeed(float value);
  
  // required float varianceX = 5 [default = -100000];
  inline bool has_variancex() const;
  inline void clear_variancex();
  static const int kVarianceXFieldNumber = 5;
  inline float variancex() const;
  inline void set_variancex(float value);
  
  // required float varianceY = 6 [default = -100000];
  inline bool has_variancey() const;
  inline void clear_variancey();
  static const int kVarianceYFieldNumber = 6;
  inline float variancey() const;
  inline void set_variancey(float value);
  
  // required float varianceXspeed = 7 [default = -100000];
  inline bool has_variancexspeed() const;
  inline void clear_variancexspeed();
  static const int kVarianceXspeedFieldNumber = 7;
  inline float variancexspeed() const;
  inline void set_variancexspeed(float value);
  
  // required float varianceYspeed = 8 [default = -100000];
  inline bool has_varianceyspeed() const;
  inline void clear_varianceyspeed();
  static const int kVarianceYspeedFieldNumber = 8;
  inline float varianceyspeed() const;
  inline void set_varianceyspeed(float value);
  
  // @@protoc_insertion_point(class_scope:Ball)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  float relativex_;
  float relativey_;
  float relativexspeed_;
  float relativeyspeed_;
  float variancex_;
  float variancey_;
  float variancexspeed_;
  float varianceyspeed_;
  friend void  protobuf_AddDesc_WorldInfo_2eproto();
  friend void protobuf_AssignDesc_WorldInfo_2eproto();
  friend void protobuf_ShutdownFile_WorldInfo_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(8 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Ball* default_instance_;
};
// -------------------------------------------------------------------

class UnknownObjects : public ::google::protobuf::Message {
 public:
  UnknownObjects();
  virtual ~UnknownObjects();
  
  UnknownObjects(const UnknownObjects& from);
  
  inline UnknownObjects& operator=(const UnknownObjects& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const UnknownObjects& default_instance();
  
  void Swap(UnknownObjects* other);
  
  // implements Message ----------------------------------------------
  
  UnknownObjects* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UnknownObjects& from);
  void MergeFrom(const UnknownObjects& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // optional float relativeX = 1 [default = -100000];
  inline bool has_relativex() const;
  inline void clear_relativex();
  static const int kRelativeXFieldNumber = 1;
  inline float relativex() const;
  inline void set_relativex(float value);
  
  // optional float relativeY = 2 [default = -100000];
  inline bool has_relativey() const;
  inline void clear_relativey();
  static const int kRelativeYFieldNumber = 2;
  inline float relativey() const;
  inline void set_relativey(float value);
  
  // optional float height = 3 [default = -100000];
  inline bool has_height() const;
  inline void clear_height();
  static const int kHeightFieldNumber = 3;
  inline float height() const;
  inline void set_height(float value);
  
  // optional float relativeXspeed = 4 [default = -100000];
  inline bool has_relativexspeed() const;
  inline void clear_relativexspeed();
  static const int kRelativeXspeedFieldNumber = 4;
  inline float relativexspeed() const;
  inline void set_relativexspeed(float value);
  
  // optional float relativeYspeed = 5 [default = -100000];
  inline bool has_relativeyspeed() const;
  inline void clear_relativeyspeed();
  static const int kRelativeYspeedFieldNumber = 5;
  inline float relativeyspeed() const;
  inline void set_relativeyspeed(float value);
  
  // @@protoc_insertion_point(class_scope:UnknownObjects)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  float relativex_;
  float relativey_;
  float height_;
  float relativexspeed_;
  float relativeyspeed_;
  friend void  protobuf_AddDesc_WorldInfo_2eproto();
  friend void protobuf_AssignDesc_WorldInfo_2eproto();
  friend void protobuf_ShutdownFile_WorldInfo_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static UnknownObjects* default_instance_;
};
// -------------------------------------------------------------------

class LocalizationData : public ::google::protobuf::Message {
 public:
  LocalizationData();
  virtual ~LocalizationData();
  
  LocalizationData(const LocalizationData& from);
  
  inline LocalizationData& operator=(const LocalizationData& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const LocalizationData& default_instance();
  
  void Swap(LocalizationData* other);
  
  // implements Message ----------------------------------------------
  
  LocalizationData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LocalizationData& from);
  void MergeFrom(const LocalizationData& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required .WorldInfo World = 1;
  inline bool has_world() const;
  inline void clear_world();
  static const int kWorldFieldNumber = 1;
  inline const ::WorldInfo& world() const;
  inline ::WorldInfo* mutable_world();
  
  // repeated .RobotPose Particles = 2;
  inline int particles_size() const;
  inline void clear_particles();
  static const int kParticlesFieldNumber = 2;
  inline const ::RobotPose& particles(int index) const;
  inline ::RobotPose* mutable_particles(int index);
  inline ::RobotPose* add_particles();
  inline const ::google::protobuf::RepeatedPtrField< ::RobotPose >&
      particles() const;
  inline ::google::protobuf::RepeatedPtrField< ::RobotPose >*
      mutable_particles();
  
  // required .RobotPose RobotPosition = 3;
  inline bool has_robotposition() const;
  inline void clear_robotposition();
  static const int kRobotPositionFieldNumber = 3;
  inline const ::RobotPose& robotposition() const;
  inline ::RobotPose* mutable_robotposition();
  
  // required .ObservationMessage Observations = 4;
  inline bool has_observations() const;
  inline void clear_observations();
  static const int kObservationsFieldNumber = 4;
  inline const ::ObservationMessage& observations() const;
  inline ::ObservationMessage* mutable_observations();
  
  // @@protoc_insertion_point(class_scope:LocalizationData)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::WorldInfo* world_;
  ::google::protobuf::RepeatedPtrField< ::RobotPose > particles_;
  ::RobotPose* robotposition_;
  ::ObservationMessage* observations_;
  friend void  protobuf_AddDesc_WorldInfo_2eproto();
  friend void protobuf_AssignDesc_WorldInfo_2eproto();
  friend void protobuf_ShutdownFile_WorldInfo_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static LocalizationData* default_instance_;
};
// -------------------------------------------------------------------

class header : public ::google::protobuf::Message {
 public:
  header();
  virtual ~header();
  
  header(const header& from);
  
  inline header& operator=(const header& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const header& default_instance();
  
  void Swap(header* other);
  
  // implements Message ----------------------------------------------
  
  header* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const header& from);
  void MergeFrom(const header& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required sint32 NextMsgByteSize = 1 [default = -1];
  inline bool has_nextmsgbytesize() const;
  inline void clear_nextmsgbytesize();
  static const int kNextMsgByteSizeFieldNumber = 1;
  inline ::google::protobuf::int32 nextmsgbytesize() const;
  inline void set_nextmsgbytesize(::google::protobuf::int32 value);
  
  // required int32 mysize = 2 [default = -1];
  inline bool has_mysize() const;
  inline void clear_mysize();
  static const int kMysizeFieldNumber = 2;
  inline ::google::protobuf::int32 mysize() const;
  inline void set_mysize(::google::protobuf::int32 value);
  
  // required bytes NextMsgName = 3 [default = "Undef"];
  inline bool has_nextmsgname() const;
  inline void clear_nextmsgname();
  static const int kNextMsgNameFieldNumber = 3;
  inline const ::std::string& nextmsgname() const;
  inline void set_nextmsgname(const ::std::string& value);
  inline void set_nextmsgname(const char* value);
  inline void set_nextmsgname(const void* value, size_t size);
  inline ::std::string* mutable_nextmsgname();
  
  // @@protoc_insertion_point(class_scope:header)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 nextmsgbytesize_;
  ::google::protobuf::int32 mysize_;
  ::std::string* nextmsgname_;
  static const ::std::string _default_nextmsgname_;
  friend void  protobuf_AddDesc_WorldInfo_2eproto();
  friend void protobuf_AssignDesc_WorldInfo_2eproto();
  friend void protobuf_ShutdownFile_WorldInfo_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static header* default_instance_;
};
// ===================================================================


// ===================================================================

// WorldInfo

// required .RobotPose myPosition = 1;
inline bool WorldInfo::has_myposition() const {
  return _has_bit(0);
}
inline void WorldInfo::clear_myposition() {
  if (myposition_ != NULL) myposition_->::RobotPose::Clear();
  _clear_bit(0);
}
inline const ::RobotPose& WorldInfo::myposition() const {
  return myposition_ != NULL ? *myposition_ : *default_instance_->myposition_;
}
inline ::RobotPose* WorldInfo::mutable_myposition() {
  _set_bit(0);
  if (myposition_ == NULL) myposition_ = new ::RobotPose;
  return myposition_;
}

// repeated .Ball Balls = 2;
inline int WorldInfo::balls_size() const {
  return balls_.size();
}
inline void WorldInfo::clear_balls() {
  balls_.Clear();
}
inline const ::Ball& WorldInfo::balls(int index) const {
  return balls_.Get(index);
}
inline ::Ball* WorldInfo::mutable_balls(int index) {
  return balls_.Mutable(index);
}
inline ::Ball* WorldInfo::add_balls() {
  return balls_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Ball >&
WorldInfo::balls() const {
  return balls_;
}
inline ::google::protobuf::RepeatedPtrField< ::Ball >*
WorldInfo::mutable_balls() {
  return &balls_;
}

// repeated .RobotPose OtherRobots = 3;
inline int WorldInfo::otherrobots_size() const {
  return otherrobots_.size();
}
inline void WorldInfo::clear_otherrobots() {
  otherrobots_.Clear();
}
inline const ::RobotPose& WorldInfo::otherrobots(int index) const {
  return otherrobots_.Get(index);
}
inline ::RobotPose* WorldInfo::mutable_otherrobots(int index) {
  return otherrobots_.Mutable(index);
}
inline ::RobotPose* WorldInfo::add_otherrobots() {
  return otherrobots_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::RobotPose >&
WorldInfo::otherrobots() const {
  return otherrobots_;
}
inline ::google::protobuf::RepeatedPtrField< ::RobotPose >*
WorldInfo::mutable_otherrobots() {
  return &otherrobots_;
}

// repeated .UnknownObjects UFOs = 4;
inline int WorldInfo::ufos_size() const {
  return ufos_.size();
}
inline void WorldInfo::clear_ufos() {
  ufos_.Clear();
}
inline const ::UnknownObjects& WorldInfo::ufos(int index) const {
  return ufos_.Get(index);
}
inline ::UnknownObjects* WorldInfo::mutable_ufos(int index) {
  return ufos_.Mutable(index);
}
inline ::UnknownObjects* WorldInfo::add_ufos() {
  return ufos_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::UnknownObjects >&
WorldInfo::ufos() const {
  return ufos_;
}
inline ::google::protobuf::RepeatedPtrField< ::UnknownObjects >*
WorldInfo::mutable_ufos() {
  return &ufos_;
}

// -------------------------------------------------------------------

// RobotPose

// required float X = 1 [default = -100000];
inline bool RobotPose::has_x() const {
  return _has_bit(0);
}
inline void RobotPose::clear_x() {
  x_ = -100000;
  _clear_bit(0);
}
inline float RobotPose::x() const {
  return x_;
}
inline void RobotPose::set_x(float value) {
  _set_bit(0);
  x_ = value;
}

// required float Y = 2 [default = -100000];
inline bool RobotPose::has_y() const {
  return _has_bit(1);
}
inline void RobotPose::clear_y() {
  y_ = -100000;
  _clear_bit(1);
}
inline float RobotPose::y() const {
  return y_;
}
inline void RobotPose::set_y(float value) {
  _set_bit(1);
  y_ = value;
}

// required float phi = 3 [default = -100000];
inline bool RobotPose::has_phi() const {
  return _has_bit(2);
}
inline void RobotPose::clear_phi() {
  phi_ = -100000;
  _clear_bit(2);
}
inline float RobotPose::phi() const {
  return phi_;
}
inline void RobotPose::set_phi(float value) {
  _set_bit(2);
  phi_ = value;
}

// required float confidence = 4 [default = -100000];
inline bool RobotPose::has_confidence() const {
  return _has_bit(3);
}
inline void RobotPose::clear_confidence() {
  confidence_ = -100000;
  _clear_bit(3);
}
inline float RobotPose::confidence() const {
  return confidence_;
}
inline void RobotPose::set_confidence(float value) {
  _set_bit(3);
  confidence_ = value;
}

// -------------------------------------------------------------------

// Ball

// required float relativeX = 1 [default = -100000];
inline bool Ball::has_relativex() const {
  return _has_bit(0);
}
inline void Ball::clear_relativex() {
  relativex_ = -100000;
  _clear_bit(0);
}
inline float Ball::relativex() const {
  return relativex_;
}
inline void Ball::set_relativex(float value) {
  _set_bit(0);
  relativex_ = value;
}

// required float relativeY = 2 [default = -100000];
inline bool Ball::has_relativey() const {
  return _has_bit(1);
}
inline void Ball::clear_relativey() {
  relativey_ = -100000;
  _clear_bit(1);
}
inline float Ball::relativey() const {
  return relativey_;
}
inline void Ball::set_relativey(float value) {
  _set_bit(1);
  relativey_ = value;
}

// required float relativeXspeed = 3 [default = -100000];
inline bool Ball::has_relativexspeed() const {
  return _has_bit(2);
}
inline void Ball::clear_relativexspeed() {
  relativexspeed_ = -100000;
  _clear_bit(2);
}
inline float Ball::relativexspeed() const {
  return relativexspeed_;
}
inline void Ball::set_relativexspeed(float value) {
  _set_bit(2);
  relativexspeed_ = value;
}

// required float relativeYspeed = 4 [default = -100000];
inline bool Ball::has_relativeyspeed() const {
  return _has_bit(3);
}
inline void Ball::clear_relativeyspeed() {
  relativeyspeed_ = -100000;
  _clear_bit(3);
}
inline float Ball::relativeyspeed() const {
  return relativeyspeed_;
}
inline void Ball::set_relativeyspeed(float value) {
  _set_bit(3);
  relativeyspeed_ = value;
}

// required float varianceX = 5 [default = -100000];
inline bool Ball::has_variancex() const {
  return _has_bit(4);
}
inline void Ball::clear_variancex() {
  variancex_ = -100000;
  _clear_bit(4);
}
inline float Ball::variancex() const {
  return variancex_;
}
inline void Ball::set_variancex(float value) {
  _set_bit(4);
  variancex_ = value;
}

// required float varianceY = 6 [default = -100000];
inline bool Ball::has_variancey() const {
  return _has_bit(5);
}
inline void Ball::clear_variancey() {
  variancey_ = -100000;
  _clear_bit(5);
}
inline float Ball::variancey() const {
  return variancey_;
}
inline void Ball::set_variancey(float value) {
  _set_bit(5);
  variancey_ = value;
}

// required float varianceXspeed = 7 [default = -100000];
inline bool Ball::has_variancexspeed() const {
  return _has_bit(6);
}
inline void Ball::clear_variancexspeed() {
  variancexspeed_ = -100000;
  _clear_bit(6);
}
inline float Ball::variancexspeed() const {
  return variancexspeed_;
}
inline void Ball::set_variancexspeed(float value) {
  _set_bit(6);
  variancexspeed_ = value;
}

// required float varianceYspeed = 8 [default = -100000];
inline bool Ball::has_varianceyspeed() const {
  return _has_bit(7);
}
inline void Ball::clear_varianceyspeed() {
  varianceyspeed_ = -100000;
  _clear_bit(7);
}
inline float Ball::varianceyspeed() const {
  return varianceyspeed_;
}
inline void Ball::set_varianceyspeed(float value) {
  _set_bit(7);
  varianceyspeed_ = value;
}

// -------------------------------------------------------------------

// UnknownObjects

// optional float relativeX = 1 [default = -100000];
inline bool UnknownObjects::has_relativex() const {
  return _has_bit(0);
}
inline void UnknownObjects::clear_relativex() {
  relativex_ = -100000;
  _clear_bit(0);
}
inline float UnknownObjects::relativex() const {
  return relativex_;
}
inline void UnknownObjects::set_relativex(float value) {
  _set_bit(0);
  relativex_ = value;
}

// optional float relativeY = 2 [default = -100000];
inline bool UnknownObjects::has_relativey() const {
  return _has_bit(1);
}
inline void UnknownObjects::clear_relativey() {
  relativey_ = -100000;
  _clear_bit(1);
}
inline float UnknownObjects::relativey() const {
  return relativey_;
}
inline void UnknownObjects::set_relativey(float value) {
  _set_bit(1);
  relativey_ = value;
}

// optional float height = 3 [default = -100000];
inline bool UnknownObjects::has_height() const {
  return _has_bit(2);
}
inline void UnknownObjects::clear_height() {
  height_ = -100000;
  _clear_bit(2);
}
inline float UnknownObjects::height() const {
  return height_;
}
inline void UnknownObjects::set_height(float value) {
  _set_bit(2);
  height_ = value;
}

// optional float relativeXspeed = 4 [default = -100000];
inline bool UnknownObjects::has_relativexspeed() const {
  return _has_bit(3);
}
inline void UnknownObjects::clear_relativexspeed() {
  relativexspeed_ = -100000;
  _clear_bit(3);
}
inline float UnknownObjects::relativexspeed() const {
  return relativexspeed_;
}
inline void UnknownObjects::set_relativexspeed(float value) {
  _set_bit(3);
  relativexspeed_ = value;
}

// optional float relativeYspeed = 5 [default = -100000];
inline bool UnknownObjects::has_relativeyspeed() const {
  return _has_bit(4);
}
inline void UnknownObjects::clear_relativeyspeed() {
  relativeyspeed_ = -100000;
  _clear_bit(4);
}
inline float UnknownObjects::relativeyspeed() const {
  return relativeyspeed_;
}
inline void UnknownObjects::set_relativeyspeed(float value) {
  _set_bit(4);
  relativeyspeed_ = value;
}

// -------------------------------------------------------------------

// LocalizationData

// required .WorldInfo World = 1;
inline bool LocalizationData::has_world() const {
  return _has_bit(0);
}
inline void LocalizationData::clear_world() {
  if (world_ != NULL) world_->::WorldInfo::Clear();
  _clear_bit(0);
}
inline const ::WorldInfo& LocalizationData::world() const {
  return world_ != NULL ? *world_ : *default_instance_->world_;
}
inline ::WorldInfo* LocalizationData::mutable_world() {
  _set_bit(0);
  if (world_ == NULL) world_ = new ::WorldInfo;
  return world_;
}

// repeated .RobotPose Particles = 2;
inline int LocalizationData::particles_size() const {
  return particles_.size();
}
inline void LocalizationData::clear_particles() {
  particles_.Clear();
}
inline const ::RobotPose& LocalizationData::particles(int index) const {
  return particles_.Get(index);
}
inline ::RobotPose* LocalizationData::mutable_particles(int index) {
  return particles_.Mutable(index);
}
inline ::RobotPose* LocalizationData::add_particles() {
  return particles_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::RobotPose >&
LocalizationData::particles() const {
  return particles_;
}
inline ::google::protobuf::RepeatedPtrField< ::RobotPose >*
LocalizationData::mutable_particles() {
  return &particles_;
}

// required .RobotPose RobotPosition = 3;
inline bool LocalizationData::has_robotposition() const {
  return _has_bit(2);
}
inline void LocalizationData::clear_robotposition() {
  if (robotposition_ != NULL) robotposition_->::RobotPose::Clear();
  _clear_bit(2);
}
inline const ::RobotPose& LocalizationData::robotposition() const {
  return robotposition_ != NULL ? *robotposition_ : *default_instance_->robotposition_;
}
inline ::RobotPose* LocalizationData::mutable_robotposition() {
  _set_bit(2);
  if (robotposition_ == NULL) robotposition_ = new ::RobotPose;
  return robotposition_;
}

// required .ObservationMessage Observations = 4;
inline bool LocalizationData::has_observations() const {
  return _has_bit(3);
}
inline void LocalizationData::clear_observations() {
  if (observations_ != NULL) observations_->::ObservationMessage::Clear();
  _clear_bit(3);
}
inline const ::ObservationMessage& LocalizationData::observations() const {
  return observations_ != NULL ? *observations_ : *default_instance_->observations_;
}
inline ::ObservationMessage* LocalizationData::mutable_observations() {
  _set_bit(3);
  if (observations_ == NULL) observations_ = new ::ObservationMessage;
  return observations_;
}

// -------------------------------------------------------------------

// header

// required sint32 NextMsgByteSize = 1 [default = -1];
inline bool header::has_nextmsgbytesize() const {
  return _has_bit(0);
}
inline void header::clear_nextmsgbytesize() {
  nextmsgbytesize_ = -1;
  _clear_bit(0);
}
inline ::google::protobuf::int32 header::nextmsgbytesize() const {
  return nextmsgbytesize_;
}
inline void header::set_nextmsgbytesize(::google::protobuf::int32 value) {
  _set_bit(0);
  nextmsgbytesize_ = value;
}

// required int32 mysize = 2 [default = -1];
inline bool header::has_mysize() const {
  return _has_bit(1);
}
inline void header::clear_mysize() {
  mysize_ = -1;
  _clear_bit(1);
}
inline ::google::protobuf::int32 header::mysize() const {
  return mysize_;
}
inline void header::set_mysize(::google::protobuf::int32 value) {
  _set_bit(1);
  mysize_ = value;
}

// required bytes NextMsgName = 3 [default = "Undef"];
inline bool header::has_nextmsgname() const {
  return _has_bit(2);
}
inline void header::clear_nextmsgname() {
  if (nextmsgname_ != &_default_nextmsgname_) {
    nextmsgname_->assign(_default_nextmsgname_);
  }
  _clear_bit(2);
}
inline const ::std::string& header::nextmsgname() const {
  return *nextmsgname_;
}
inline void header::set_nextmsgname(const ::std::string& value) {
  _set_bit(2);
  if (nextmsgname_ == &_default_nextmsgname_) {
    nextmsgname_ = new ::std::string;
  }
  nextmsgname_->assign(value);
}
inline void header::set_nextmsgname(const char* value) {
  _set_bit(2);
  if (nextmsgname_ == &_default_nextmsgname_) {
    nextmsgname_ = new ::std::string;
  }
  nextmsgname_->assign(value);
}
inline void header::set_nextmsgname(const void* value, size_t size) {
  _set_bit(2);
  if (nextmsgname_ == &_default_nextmsgname_) {
    nextmsgname_ = new ::std::string;
  }
  nextmsgname_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* header::mutable_nextmsgname() {
  _set_bit(2);
  if (nextmsgname_ == &_default_nextmsgname_) {
    nextmsgname_ = new ::std::string(_default_nextmsgname_);
  }
  return nextmsgname_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_WorldInfo_2eproto__INCLUDED