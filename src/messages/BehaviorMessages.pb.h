// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BehaviorMessages.proto

#ifndef PROTOBUF_BehaviorMessages_2eproto__INCLUDED
#define PROTOBUF_BehaviorMessages_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "VisionObservations.pb.h"
#include "WorldInfo.pb.h"
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_BehaviorMessages_2eproto();
void protobuf_AssignDesc_BehaviorMessages_2eproto();
void protobuf_ShutdownFile_BehaviorMessages_2eproto();

class ActionDescription;
class BodyControlMessage;
class HeadControlMessage;

enum ActionDescription_URGENCY_LEVELS {
  ActionDescription_URGENCY_LEVELS_SOMEDAY = 0,
  ActionDescription_URGENCY_LEVELS_SLOW = 1,
  ActionDescription_URGENCY_LEVELS_BEAM_ME_UP = 2
};
bool ActionDescription_URGENCY_LEVELS_IsValid(int value);
const ActionDescription_URGENCY_LEVELS ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_MIN = ActionDescription_URGENCY_LEVELS_SOMEDAY;
const ActionDescription_URGENCY_LEVELS ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_MAX = ActionDescription_URGENCY_LEVELS_BEAM_ME_UP;
const int ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_ARRAYSIZE = ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_MAX + 1;

const ::google::protobuf::EnumDescriptor* ActionDescription_URGENCY_LEVELS_descriptor();
inline const ::std::string& ActionDescription_URGENCY_LEVELS_Name(ActionDescription_URGENCY_LEVELS value) {
  return ::google::protobuf::internal::NameOfEnum(
    ActionDescription_URGENCY_LEVELS_descriptor(), value);
}
inline bool ActionDescription_URGENCY_LEVELS_Parse(
    const ::std::string& name, ActionDescription_URGENCY_LEVELS* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ActionDescription_URGENCY_LEVELS>(
    ActionDescription_URGENCY_LEVELS_descriptor(), name, value);
}
enum BodyControlMessage_BODYACTIONS {
  BodyControlMessage_BODYACTIONS_NOTHING = 0,
  BodyControlMessage_BODYACTIONS_STAND = 1,
  BodyControlMessage_BODYACTIONS_DOACTION = 2,
  BodyControlMessage_BODYACTIONS_BALLAPPROACH = 3,
  BodyControlMessage_BODYACTIONS_BALLAPPROACH_SUPPORT = 4,
  BodyControlMessage_BODYACTIONS_GOTO = 5,
  BodyControlMessage_BODYACTIONS_DEMO = 6
};
bool BodyControlMessage_BODYACTIONS_IsValid(int value);
const BodyControlMessage_BODYACTIONS BodyControlMessage_BODYACTIONS_BODYACTIONS_MIN = BodyControlMessage_BODYACTIONS_NOTHING;
const BodyControlMessage_BODYACTIONS BodyControlMessage_BODYACTIONS_BODYACTIONS_MAX = BodyControlMessage_BODYACTIONS_DEMO;
const int BodyControlMessage_BODYACTIONS_BODYACTIONS_ARRAYSIZE = BodyControlMessage_BODYACTIONS_BODYACTIONS_MAX + 1;

const ::google::protobuf::EnumDescriptor* BodyControlMessage_BODYACTIONS_descriptor();
inline const ::std::string& BodyControlMessage_BODYACTIONS_Name(BodyControlMessage_BODYACTIONS value) {
  return ::google::protobuf::internal::NameOfEnum(
    BodyControlMessage_BODYACTIONS_descriptor(), value);
}
inline bool BodyControlMessage_BODYACTIONS_Parse(
    const ::std::string& name, BodyControlMessage_BODYACTIONS* value) {
  return ::google::protobuf::internal::ParseNamedEnum<BodyControlMessage_BODYACTIONS>(
    BodyControlMessage_BODYACTIONS_descriptor(), name, value);
}
enum HeadControlMessage_HEADACTIONS {
  HeadControlMessage_HEADACTIONS_NOTHING = 0,
  HeadControlMessage_HEADACTIONS_FROWN = 1,
  HeadControlMessage_HEADACTIONS_SCAN_AND_TRACK_FOR_BALL = 2,
  HeadControlMessage_HEADACTIONS_LOCALIZE = 3,
  HeadControlMessage_HEADACTIONS_SMART_SELECT = 4
};
bool HeadControlMessage_HEADACTIONS_IsValid(int value);
const HeadControlMessage_HEADACTIONS HeadControlMessage_HEADACTIONS_HEADACTIONS_MIN = HeadControlMessage_HEADACTIONS_NOTHING;
const HeadControlMessage_HEADACTIONS HeadControlMessage_HEADACTIONS_HEADACTIONS_MAX = HeadControlMessage_HEADACTIONS_SMART_SELECT;
const int HeadControlMessage_HEADACTIONS_HEADACTIONS_ARRAYSIZE = HeadControlMessage_HEADACTIONS_HEADACTIONS_MAX + 1;

const ::google::protobuf::EnumDescriptor* HeadControlMessage_HEADACTIONS_descriptor();
inline const ::std::string& HeadControlMessage_HEADACTIONS_Name(HeadControlMessage_HEADACTIONS value) {
  return ::google::protobuf::internal::NameOfEnum(
    HeadControlMessage_HEADACTIONS_descriptor(), value);
}
inline bool HeadControlMessage_HEADACTIONS_Parse(
    const ::std::string& name, HeadControlMessage_HEADACTIONS* value) {
  return ::google::protobuf::internal::ParseNamedEnum<HeadControlMessage_HEADACTIONS>(
    HeadControlMessage_HEADACTIONS_descriptor(), name, value);
}
// ===================================================================

class ActionDescription : public ::google::protobuf::Message {
 public:
  ActionDescription();
  virtual ~ActionDescription();
  
  ActionDescription(const ActionDescription& from);
  
  inline ActionDescription& operator=(const ActionDescription& from) {
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
  static const ActionDescription& default_instance();
  
  void Swap(ActionDescription* other);
  
  // implements Message ----------------------------------------------
  
  ActionDescription* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ActionDescription& from);
  void MergeFrom(const ActionDescription& from);
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
  
  typedef ActionDescription_URGENCY_LEVELS URGENCY_LEVELS;
  static const URGENCY_LEVELS SOMEDAY = ActionDescription_URGENCY_LEVELS_SOMEDAY;
  static const URGENCY_LEVELS SLOW = ActionDescription_URGENCY_LEVELS_SLOW;
  static const URGENCY_LEVELS BEAM_ME_UP = ActionDescription_URGENCY_LEVELS_BEAM_ME_UP;
  static inline bool URGENCY_LEVELS_IsValid(int value) {
    return ActionDescription_URGENCY_LEVELS_IsValid(value);
  }
  static const URGENCY_LEVELS URGENCY_LEVELS_MIN =
    ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_MIN;
  static const URGENCY_LEVELS URGENCY_LEVELS_MAX =
    ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_MAX;
  static const int URGENCY_LEVELS_ARRAYSIZE =
    ActionDescription_URGENCY_LEVELS_URGENCY_LEVELS_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  URGENCY_LEVELS_descriptor() {
    return ActionDescription_URGENCY_LEVELS_descriptor();
  }
  static inline const ::std::string& URGENCY_LEVELS_Name(URGENCY_LEVELS value) {
    return ActionDescription_URGENCY_LEVELS_Name(value);
  }
  static inline bool URGENCY_LEVELS_Parse(const ::std::string& name,
      URGENCY_LEVELS* value) {
    return ActionDescription_URGENCY_LEVELS_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required uint32 action = 1;
  inline bool has_action() const;
  inline void clear_action();
  static const int kActionFieldNumber = 1;
  inline ::google::protobuf::uint32 action() const;
  inline void set_action(::google::protobuf::uint32 value);
  
  // required uint32 urgency = 2;
  inline bool has_urgency() const;
  inline void clear_urgency();
  static const int kUrgencyFieldNumber = 2;
  inline ::google::protobuf::uint32 urgency() const;
  inline void set_urgency(::google::protobuf::uint32 value);
  
  // required string description = 3;
  inline bool has_description() const;
  inline void clear_description();
  static const int kDescriptionFieldNumber = 3;
  inline const ::std::string& description() const;
  inline void set_description(const ::std::string& value);
  inline void set_description(const char* value);
  inline void set_description(const char* value, size_t size);
  inline ::std::string* mutable_description();
  inline ::std::string* release_description();
  
  // required float x = 4;
  inline bool has_x() const;
  inline void clear_x();
  static const int kXFieldNumber = 4;
  inline float x() const;
  inline void set_x(float value);
  
  // required float y = 5;
  inline bool has_y() const;
  inline void clear_y();
  static const int kYFieldNumber = 5;
  inline float y() const;
  inline void set_y(float value);
  
  // required float phi = 6;
  inline bool has_phi() const;
  inline void clear_phi();
  static const int kPhiFieldNumber = 6;
  inline float phi() const;
  inline void set_phi(float value);
  
  // @@protoc_insertion_point(class_scope:ActionDescription)
 private:
  inline void set_has_action();
  inline void clear_has_action();
  inline void set_has_urgency();
  inline void clear_has_urgency();
  inline void set_has_description();
  inline void clear_has_description();
  inline void set_has_x();
  inline void clear_has_x();
  inline void set_has_y();
  inline void clear_has_y();
  inline void set_has_phi();
  inline void clear_has_phi();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::uint32 action_;
  ::google::protobuf::uint32 urgency_;
  ::std::string* description_;
  float x_;
  float y_;
  float phi_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(6 + 31) / 32];
  
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  void InitAsDefaultInstance();
  static ActionDescription* default_instance_;
};
// -------------------------------------------------------------------

class BodyControlMessage : public ::google::protobuf::Message {
 public:
  BodyControlMessage();
  virtual ~BodyControlMessage();
  
  BodyControlMessage(const BodyControlMessage& from);
  
  inline BodyControlMessage& operator=(const BodyControlMessage& from) {
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
  static const BodyControlMessage& default_instance();
  
  void Swap(BodyControlMessage* other);
  
  // implements Message ----------------------------------------------
  
  BodyControlMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const BodyControlMessage& from);
  void MergeFrom(const BodyControlMessage& from);
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
  
  typedef BodyControlMessage_BODYACTIONS BODYACTIONS;
  static const BODYACTIONS NOTHING = BodyControlMessage_BODYACTIONS_NOTHING;
  static const BODYACTIONS STAND = BodyControlMessage_BODYACTIONS_STAND;
  static const BODYACTIONS DOACTION = BodyControlMessage_BODYACTIONS_DOACTION;
  static const BODYACTIONS BALLAPPROACH = BodyControlMessage_BODYACTIONS_BALLAPPROACH;
  static const BODYACTIONS BALLAPPROACH_SUPPORT = BodyControlMessage_BODYACTIONS_BALLAPPROACH_SUPPORT;
  static const BODYACTIONS GOTO = BodyControlMessage_BODYACTIONS_GOTO;
  static const BODYACTIONS DEMO = BodyControlMessage_BODYACTIONS_DEMO;
  static inline bool BODYACTIONS_IsValid(int value) {
    return BodyControlMessage_BODYACTIONS_IsValid(value);
  }
  static const BODYACTIONS BODYACTIONS_MIN =
    BodyControlMessage_BODYACTIONS_BODYACTIONS_MIN;
  static const BODYACTIONS BODYACTIONS_MAX =
    BodyControlMessage_BODYACTIONS_BODYACTIONS_MAX;
  static const int BODYACTIONS_ARRAYSIZE =
    BodyControlMessage_BODYACTIONS_BODYACTIONS_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  BODYACTIONS_descriptor() {
    return BodyControlMessage_BODYACTIONS_descriptor();
  }
  static inline const ::std::string& BODYACTIONS_Name(BODYACTIONS value) {
    return BodyControlMessage_BODYACTIONS_Name(value);
  }
  static inline bool BODYACTIONS_Parse(const ::std::string& name,
      BODYACTIONS* value) {
    return BodyControlMessage_BODYACTIONS_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required .ActionDescription task = 1;
  inline bool has_task() const;
  inline void clear_task();
  static const int kTaskFieldNumber = 1;
  inline const ::ActionDescription& task() const;
  inline ::ActionDescription* mutable_task();
  inline ::ActionDescription* release_task();
  
  // @@protoc_insertion_point(class_scope:BodyControlMessage)
 private:
  inline void set_has_task();
  inline void clear_has_task();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::ActionDescription* task_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  void InitAsDefaultInstance();
  static BodyControlMessage* default_instance_;
};
// -------------------------------------------------------------------

class HeadControlMessage : public ::google::protobuf::Message {
 public:
  HeadControlMessage();
  virtual ~HeadControlMessage();
  
  HeadControlMessage(const HeadControlMessage& from);
  
  inline HeadControlMessage& operator=(const HeadControlMessage& from) {
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
  static const HeadControlMessage& default_instance();
  
  void Swap(HeadControlMessage* other);
  
  // implements Message ----------------------------------------------
  
  HeadControlMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const HeadControlMessage& from);
  void MergeFrom(const HeadControlMessage& from);
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
  
  typedef HeadControlMessage_HEADACTIONS HEADACTIONS;
  static const HEADACTIONS NOTHING = HeadControlMessage_HEADACTIONS_NOTHING;
  static const HEADACTIONS FROWN = HeadControlMessage_HEADACTIONS_FROWN;
  static const HEADACTIONS SCAN_AND_TRACK_FOR_BALL = HeadControlMessage_HEADACTIONS_SCAN_AND_TRACK_FOR_BALL;
  static const HEADACTIONS LOCALIZE = HeadControlMessage_HEADACTIONS_LOCALIZE;
  static const HEADACTIONS SMART_SELECT = HeadControlMessage_HEADACTIONS_SMART_SELECT;
  static inline bool HEADACTIONS_IsValid(int value) {
    return HeadControlMessage_HEADACTIONS_IsValid(value);
  }
  static const HEADACTIONS HEADACTIONS_MIN =
    HeadControlMessage_HEADACTIONS_HEADACTIONS_MIN;
  static const HEADACTIONS HEADACTIONS_MAX =
    HeadControlMessage_HEADACTIONS_HEADACTIONS_MAX;
  static const int HEADACTIONS_ARRAYSIZE =
    HeadControlMessage_HEADACTIONS_HEADACTIONS_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  HEADACTIONS_descriptor() {
    return HeadControlMessage_HEADACTIONS_descriptor();
  }
  static inline const ::std::string& HEADACTIONS_Name(HEADACTIONS value) {
    return HeadControlMessage_HEADACTIONS_Name(value);
  }
  static inline bool HEADACTIONS_Parse(const ::std::string& name,
      HEADACTIONS* value) {
    return HeadControlMessage_HEADACTIONS_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required .ActionDescription task = 1;
  inline bool has_task() const;
  inline void clear_task();
  static const int kTaskFieldNumber = 1;
  inline const ::ActionDescription& task() const;
  inline ::ActionDescription* mutable_task();
  inline ::ActionDescription* release_task();
  
  // @@protoc_insertion_point(class_scope:HeadControlMessage)
 private:
  inline void set_has_task();
  inline void clear_has_task();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::ActionDescription* task_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  void InitAsDefaultInstance();
  static HeadControlMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// ActionDescription

// required uint32 action = 1;
inline bool ActionDescription::has_action() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ActionDescription::set_has_action() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ActionDescription::clear_has_action() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ActionDescription::clear_action() {
  action_ = 0u;
  clear_has_action();
}
inline ::google::protobuf::uint32 ActionDescription::action() const {
  return action_;
}
inline void ActionDescription::set_action(::google::protobuf::uint32 value) {
  set_has_action();
  action_ = value;
}

// required uint32 urgency = 2;
inline bool ActionDescription::has_urgency() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ActionDescription::set_has_urgency() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ActionDescription::clear_has_urgency() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ActionDescription::clear_urgency() {
  urgency_ = 0u;
  clear_has_urgency();
}
inline ::google::protobuf::uint32 ActionDescription::urgency() const {
  return urgency_;
}
inline void ActionDescription::set_urgency(::google::protobuf::uint32 value) {
  set_has_urgency();
  urgency_ = value;
}

// required string description = 3;
inline bool ActionDescription::has_description() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ActionDescription::set_has_description() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ActionDescription::clear_has_description() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ActionDescription::clear_description() {
  if (description_ != &::google::protobuf::internal::kEmptyString) {
    description_->clear();
  }
  clear_has_description();
}
inline const ::std::string& ActionDescription::description() const {
  return *description_;
}
inline void ActionDescription::set_description(const ::std::string& value) {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  description_->assign(value);
}
inline void ActionDescription::set_description(const char* value) {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  description_->assign(value);
}
inline void ActionDescription::set_description(const char* value, size_t size) {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  description_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ActionDescription::mutable_description() {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  return description_;
}
inline ::std::string* ActionDescription::release_description() {
  clear_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = description_;
    description_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required float x = 4;
inline bool ActionDescription::has_x() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ActionDescription::set_has_x() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ActionDescription::clear_has_x() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ActionDescription::clear_x() {
  x_ = 0;
  clear_has_x();
}
inline float ActionDescription::x() const {
  return x_;
}
inline void ActionDescription::set_x(float value) {
  set_has_x();
  x_ = value;
}

// required float y = 5;
inline bool ActionDescription::has_y() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ActionDescription::set_has_y() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ActionDescription::clear_has_y() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ActionDescription::clear_y() {
  y_ = 0;
  clear_has_y();
}
inline float ActionDescription::y() const {
  return y_;
}
inline void ActionDescription::set_y(float value) {
  set_has_y();
  y_ = value;
}

// required float phi = 6;
inline bool ActionDescription::has_phi() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void ActionDescription::set_has_phi() {
  _has_bits_[0] |= 0x00000020u;
}
inline void ActionDescription::clear_has_phi() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void ActionDescription::clear_phi() {
  phi_ = 0;
  clear_has_phi();
}
inline float ActionDescription::phi() const {
  return phi_;
}
inline void ActionDescription::set_phi(float value) {
  set_has_phi();
  phi_ = value;
}

// -------------------------------------------------------------------

// BodyControlMessage

// required .ActionDescription task = 1;
inline bool BodyControlMessage::has_task() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void BodyControlMessage::set_has_task() {
  _has_bits_[0] |= 0x00000001u;
}
inline void BodyControlMessage::clear_has_task() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void BodyControlMessage::clear_task() {
  if (task_ != NULL) task_->::ActionDescription::Clear();
  clear_has_task();
}
inline const ::ActionDescription& BodyControlMessage::task() const {
  return task_ != NULL ? *task_ : *default_instance_->task_;
}
inline ::ActionDescription* BodyControlMessage::mutable_task() {
  set_has_task();
  if (task_ == NULL) task_ = new ::ActionDescription;
  return task_;
}
inline ::ActionDescription* BodyControlMessage::release_task() {
  clear_has_task();
  ::ActionDescription* temp = task_;
  task_ = NULL;
  return temp;
}

// -------------------------------------------------------------------

// HeadControlMessage

// required .ActionDescription task = 1;
inline bool HeadControlMessage::has_task() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void HeadControlMessage::set_has_task() {
  _has_bits_[0] |= 0x00000001u;
}
inline void HeadControlMessage::clear_has_task() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void HeadControlMessage::clear_task() {
  if (task_ != NULL) task_->::ActionDescription::Clear();
  clear_has_task();
}
inline const ::ActionDescription& HeadControlMessage::task() const {
  return task_ != NULL ? *task_ : *default_instance_->task_;
}
inline ::ActionDescription* HeadControlMessage::mutable_task() {
  set_has_task();
  if (task_ == NULL) task_ = new ::ActionDescription;
  return task_;
}
inline ::ActionDescription* HeadControlMessage::release_task() {
  clear_has_task();
  ::ActionDescription* temp = task_;
  task_ = NULL;
  return temp;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ActionDescription_URGENCY_LEVELS>() {
  return ::ActionDescription_URGENCY_LEVELS_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::BodyControlMessage_BODYACTIONS>() {
  return ::BodyControlMessage_BODYACTIONS_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::HeadControlMessage_HEADACTIONS>() {
  return ::HeadControlMessage_HEADACTIONS_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_BehaviorMessages_2eproto__INCLUDED
