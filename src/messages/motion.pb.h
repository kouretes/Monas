// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: motion.proto

#ifndef PROTOBUF_motion_2eproto__INCLUDED
#define PROTOBUF_motion_2eproto__INCLUDED

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
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_motion_2eproto();
void protobuf_AssignDesc_motion_2eproto();
void protobuf_ShutdownFile_motion_2eproto();

class MotionWalkMessage;
class MotionHeadMessage;
class MotionActionMessage;
class MotionStateMessage;

enum MotionStateMessage_ActionType {
  MotionStateMessage_ActionType_WALKING = 0,
  MotionStateMessage_ActionType_ACTION = 1,
  MotionStateMessage_ActionType_FALL = 2,
  MotionStateMessage_ActionType_STANDUP = 3,
  MotionStateMessage_ActionType_IDLE = 4
};
bool MotionStateMessage_ActionType_IsValid(int value);
const MotionStateMessage_ActionType MotionStateMessage_ActionType_ActionType_MIN = MotionStateMessage_ActionType_WALKING;
const MotionStateMessage_ActionType MotionStateMessage_ActionType_ActionType_MAX = MotionStateMessage_ActionType_IDLE;
const int MotionStateMessage_ActionType_ActionType_ARRAYSIZE = MotionStateMessage_ActionType_ActionType_MAX + 1;

const ::google::protobuf::EnumDescriptor* MotionStateMessage_ActionType_descriptor();
inline const ::std::string& MotionStateMessage_ActionType_Name(MotionStateMessage_ActionType value) {
  return ::google::protobuf::internal::NameOfEnum(
    MotionStateMessage_ActionType_descriptor(), value);
}
inline bool MotionStateMessage_ActionType_Parse(
    const ::std::string& name, MotionStateMessage_ActionType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MotionStateMessage_ActionType>(
    MotionStateMessage_ActionType_descriptor(), name, value);
}
// ===================================================================

class MotionWalkMessage : public ::google::protobuf::Message {
 public:
  MotionWalkMessage();
  virtual ~MotionWalkMessage();
  
  MotionWalkMessage(const MotionWalkMessage& from);
  
  inline MotionWalkMessage& operator=(const MotionWalkMessage& from) {
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
  static const MotionWalkMessage& default_instance();
  
  void Swap(MotionWalkMessage* other);
  
  // implements Message ----------------------------------------------
  
  MotionWalkMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MotionWalkMessage& from);
  void MergeFrom(const MotionWalkMessage& from);
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
  
  // required string command = 6 [default = ""];
  inline bool has_command() const;
  inline void clear_command();
  static const int kCommandFieldNumber = 6;
  inline const ::std::string& command() const;
  inline void set_command(const ::std::string& value);
  inline void set_command(const char* value);
  inline void set_command(const char* value, size_t size);
  inline ::std::string* mutable_command();
  inline ::std::string* release_command();
  
  // repeated float parameter = 7;
  inline int parameter_size() const;
  inline void clear_parameter();
  static const int kParameterFieldNumber = 7;
  inline float parameter(int index) const;
  inline void set_parameter(int index, float value);
  inline void add_parameter(float value);
  inline const ::google::protobuf::RepeatedField< float >&
      parameter() const;
  inline ::google::protobuf::RepeatedField< float >*
      mutable_parameter();
  
  // @@protoc_insertion_point(class_scope:MotionWalkMessage)
 private:
  inline void set_has_command();
  inline void clear_has_command();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* command_;
  ::google::protobuf::RepeatedField< float > parameter_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_motion_2eproto();
  friend void protobuf_AssignDesc_motion_2eproto();
  friend void protobuf_ShutdownFile_motion_2eproto();
  
  void InitAsDefaultInstance();
  static MotionWalkMessage* default_instance_;
};
// -------------------------------------------------------------------

class MotionHeadMessage : public ::google::protobuf::Message {
 public:
  MotionHeadMessage();
  virtual ~MotionHeadMessage();
  
  MotionHeadMessage(const MotionHeadMessage& from);
  
  inline MotionHeadMessage& operator=(const MotionHeadMessage& from) {
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
  static const MotionHeadMessage& default_instance();
  
  void Swap(MotionHeadMessage* other);
  
  // implements Message ----------------------------------------------
  
  MotionHeadMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MotionHeadMessage& from);
  void MergeFrom(const MotionHeadMessage& from);
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
  
  // required string command = 6 [default = ""];
  inline bool has_command() const;
  inline void clear_command();
  static const int kCommandFieldNumber = 6;
  inline const ::std::string& command() const;
  inline void set_command(const ::std::string& value);
  inline void set_command(const char* value);
  inline void set_command(const char* value, size_t size);
  inline ::std::string* mutable_command();
  inline ::std::string* release_command();
  
  // repeated float parameter = 7;
  inline int parameter_size() const;
  inline void clear_parameter();
  static const int kParameterFieldNumber = 7;
  inline float parameter(int index) const;
  inline void set_parameter(int index, float value);
  inline void add_parameter(float value);
  inline const ::google::protobuf::RepeatedField< float >&
      parameter() const;
  inline ::google::protobuf::RepeatedField< float >*
      mutable_parameter();
  
  // @@protoc_insertion_point(class_scope:MotionHeadMessage)
 private:
  inline void set_has_command();
  inline void clear_has_command();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* command_;
  ::google::protobuf::RepeatedField< float > parameter_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_motion_2eproto();
  friend void protobuf_AssignDesc_motion_2eproto();
  friend void protobuf_ShutdownFile_motion_2eproto();
  
  void InitAsDefaultInstance();
  static MotionHeadMessage* default_instance_;
};
// -------------------------------------------------------------------

class MotionActionMessage : public ::google::protobuf::Message {
 public:
  MotionActionMessage();
  virtual ~MotionActionMessage();
  
  MotionActionMessage(const MotionActionMessage& from);
  
  inline MotionActionMessage& operator=(const MotionActionMessage& from) {
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
  static const MotionActionMessage& default_instance();
  
  void Swap(MotionActionMessage* other);
  
  // implements Message ----------------------------------------------
  
  MotionActionMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MotionActionMessage& from);
  void MergeFrom(const MotionActionMessage& from);
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
  
  // required string command = 6 [default = ""];
  inline bool has_command() const;
  inline void clear_command();
  static const int kCommandFieldNumber = 6;
  inline const ::std::string& command() const;
  inline void set_command(const ::std::string& value);
  inline void set_command(const char* value);
  inline void set_command(const char* value, size_t size);
  inline ::std::string* mutable_command();
  inline ::std::string* release_command();
  
  // repeated float parameter = 7;
  inline int parameter_size() const;
  inline void clear_parameter();
  static const int kParameterFieldNumber = 7;
  inline float parameter(int index) const;
  inline void set_parameter(int index, float value);
  inline void add_parameter(float value);
  inline const ::google::protobuf::RepeatedField< float >&
      parameter() const;
  inline ::google::protobuf::RepeatedField< float >*
      mutable_parameter();
  
  // @@protoc_insertion_point(class_scope:MotionActionMessage)
 private:
  inline void set_has_command();
  inline void clear_has_command();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* command_;
  ::google::protobuf::RepeatedField< float > parameter_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_motion_2eproto();
  friend void protobuf_AssignDesc_motion_2eproto();
  friend void protobuf_ShutdownFile_motion_2eproto();
  
  void InitAsDefaultInstance();
  static MotionActionMessage* default_instance_;
};
// -------------------------------------------------------------------

class MotionStateMessage : public ::google::protobuf::Message {
 public:
  MotionStateMessage();
  virtual ~MotionStateMessage();
  
  MotionStateMessage(const MotionStateMessage& from);
  
  inline MotionStateMessage& operator=(const MotionStateMessage& from) {
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
  static const MotionStateMessage& default_instance();
  
  void Swap(MotionStateMessage* other);
  
  // implements Message ----------------------------------------------
  
  MotionStateMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MotionStateMessage& from);
  void MergeFrom(const MotionStateMessage& from);
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
  
  typedef MotionStateMessage_ActionType ActionType;
  static const ActionType WALKING = MotionStateMessage_ActionType_WALKING;
  static const ActionType ACTION = MotionStateMessage_ActionType_ACTION;
  static const ActionType FALL = MotionStateMessage_ActionType_FALL;
  static const ActionType STANDUP = MotionStateMessage_ActionType_STANDUP;
  static const ActionType IDLE = MotionStateMessage_ActionType_IDLE;
  static inline bool ActionType_IsValid(int value) {
    return MotionStateMessage_ActionType_IsValid(value);
  }
  static const ActionType ActionType_MIN =
    MotionStateMessage_ActionType_ActionType_MIN;
  static const ActionType ActionType_MAX =
    MotionStateMessage_ActionType_ActionType_MAX;
  static const int ActionType_ARRAYSIZE =
    MotionStateMessage_ActionType_ActionType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  ActionType_descriptor() {
    return MotionStateMessage_ActionType_descriptor();
  }
  static inline const ::std::string& ActionType_Name(ActionType value) {
    return MotionStateMessage_ActionType_Name(value);
  }
  static inline bool ActionType_Parse(const ::std::string& name,
      ActionType* value) {
    return MotionStateMessage_ActionType_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required .MotionStateMessage.ActionType type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline ::MotionStateMessage_ActionType type() const;
  inline void set_type(::MotionStateMessage_ActionType value);
  
  // required string detail = 2;
  inline bool has_detail() const;
  inline void clear_detail();
  static const int kDetailFieldNumber = 2;
  inline const ::std::string& detail() const;
  inline void set_detail(const ::std::string& value);
  inline void set_detail(const char* value);
  inline void set_detail(const char* value, size_t size);
  inline ::std::string* mutable_detail();
  inline ::std::string* release_detail();
  
  // @@protoc_insertion_point(class_scope:MotionStateMessage)
 private:
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_detail();
  inline void clear_has_detail();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* detail_;
  int type_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_motion_2eproto();
  friend void protobuf_AssignDesc_motion_2eproto();
  friend void protobuf_ShutdownFile_motion_2eproto();
  
  void InitAsDefaultInstance();
  static MotionStateMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// MotionWalkMessage

// required string command = 6 [default = ""];
inline bool MotionWalkMessage::has_command() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MotionWalkMessage::set_has_command() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MotionWalkMessage::clear_has_command() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MotionWalkMessage::clear_command() {
  if (command_ != &::google::protobuf::internal::kEmptyString) {
    command_->clear();
  }
  clear_has_command();
}
inline const ::std::string& MotionWalkMessage::command() const {
  return *command_;
}
inline void MotionWalkMessage::set_command(const ::std::string& value) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(value);
}
inline void MotionWalkMessage::set_command(const char* value) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(value);
}
inline void MotionWalkMessage::set_command(const char* value, size_t size) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* MotionWalkMessage::mutable_command() {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  return command_;
}
inline ::std::string* MotionWalkMessage::release_command() {
  clear_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = command_;
    command_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated float parameter = 7;
inline int MotionWalkMessage::parameter_size() const {
  return parameter_.size();
}
inline void MotionWalkMessage::clear_parameter() {
  parameter_.Clear();
}
inline float MotionWalkMessage::parameter(int index) const {
  return parameter_.Get(index);
}
inline void MotionWalkMessage::set_parameter(int index, float value) {
  parameter_.Set(index, value);
}
inline void MotionWalkMessage::add_parameter(float value) {
  parameter_.Add(value);
}
inline const ::google::protobuf::RepeatedField< float >&
MotionWalkMessage::parameter() const {
  return parameter_;
}
inline ::google::protobuf::RepeatedField< float >*
MotionWalkMessage::mutable_parameter() {
  return &parameter_;
}

// -------------------------------------------------------------------

// MotionHeadMessage

// required string command = 6 [default = ""];
inline bool MotionHeadMessage::has_command() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MotionHeadMessage::set_has_command() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MotionHeadMessage::clear_has_command() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MotionHeadMessage::clear_command() {
  if (command_ != &::google::protobuf::internal::kEmptyString) {
    command_->clear();
  }
  clear_has_command();
}
inline const ::std::string& MotionHeadMessage::command() const {
  return *command_;
}
inline void MotionHeadMessage::set_command(const ::std::string& value) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(value);
}
inline void MotionHeadMessage::set_command(const char* value) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(value);
}
inline void MotionHeadMessage::set_command(const char* value, size_t size) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* MotionHeadMessage::mutable_command() {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  return command_;
}
inline ::std::string* MotionHeadMessage::release_command() {
  clear_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = command_;
    command_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated float parameter = 7;
inline int MotionHeadMessage::parameter_size() const {
  return parameter_.size();
}
inline void MotionHeadMessage::clear_parameter() {
  parameter_.Clear();
}
inline float MotionHeadMessage::parameter(int index) const {
  return parameter_.Get(index);
}
inline void MotionHeadMessage::set_parameter(int index, float value) {
  parameter_.Set(index, value);
}
inline void MotionHeadMessage::add_parameter(float value) {
  parameter_.Add(value);
}
inline const ::google::protobuf::RepeatedField< float >&
MotionHeadMessage::parameter() const {
  return parameter_;
}
inline ::google::protobuf::RepeatedField< float >*
MotionHeadMessage::mutable_parameter() {
  return &parameter_;
}

// -------------------------------------------------------------------

// MotionActionMessage

// required string command = 6 [default = ""];
inline bool MotionActionMessage::has_command() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MotionActionMessage::set_has_command() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MotionActionMessage::clear_has_command() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MotionActionMessage::clear_command() {
  if (command_ != &::google::protobuf::internal::kEmptyString) {
    command_->clear();
  }
  clear_has_command();
}
inline const ::std::string& MotionActionMessage::command() const {
  return *command_;
}
inline void MotionActionMessage::set_command(const ::std::string& value) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(value);
}
inline void MotionActionMessage::set_command(const char* value) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(value);
}
inline void MotionActionMessage::set_command(const char* value, size_t size) {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  command_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* MotionActionMessage::mutable_command() {
  set_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    command_ = new ::std::string;
  }
  return command_;
}
inline ::std::string* MotionActionMessage::release_command() {
  clear_has_command();
  if (command_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = command_;
    command_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated float parameter = 7;
inline int MotionActionMessage::parameter_size() const {
  return parameter_.size();
}
inline void MotionActionMessage::clear_parameter() {
  parameter_.Clear();
}
inline float MotionActionMessage::parameter(int index) const {
  return parameter_.Get(index);
}
inline void MotionActionMessage::set_parameter(int index, float value) {
  parameter_.Set(index, value);
}
inline void MotionActionMessage::add_parameter(float value) {
  parameter_.Add(value);
}
inline const ::google::protobuf::RepeatedField< float >&
MotionActionMessage::parameter() const {
  return parameter_;
}
inline ::google::protobuf::RepeatedField< float >*
MotionActionMessage::mutable_parameter() {
  return &parameter_;
}

// -------------------------------------------------------------------

// MotionStateMessage

// required .MotionStateMessage.ActionType type = 1;
inline bool MotionStateMessage::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MotionStateMessage::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MotionStateMessage::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MotionStateMessage::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::MotionStateMessage_ActionType MotionStateMessage::type() const {
  return static_cast< ::MotionStateMessage_ActionType >(type_);
}
inline void MotionStateMessage::set_type(::MotionStateMessage_ActionType value) {
  GOOGLE_DCHECK(::MotionStateMessage_ActionType_IsValid(value));
  set_has_type();
  type_ = value;
}

// required string detail = 2;
inline bool MotionStateMessage::has_detail() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MotionStateMessage::set_has_detail() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MotionStateMessage::clear_has_detail() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MotionStateMessage::clear_detail() {
  if (detail_ != &::google::protobuf::internal::kEmptyString) {
    detail_->clear();
  }
  clear_has_detail();
}
inline const ::std::string& MotionStateMessage::detail() const {
  return *detail_;
}
inline void MotionStateMessage::set_detail(const ::std::string& value) {
  set_has_detail();
  if (detail_ == &::google::protobuf::internal::kEmptyString) {
    detail_ = new ::std::string;
  }
  detail_->assign(value);
}
inline void MotionStateMessage::set_detail(const char* value) {
  set_has_detail();
  if (detail_ == &::google::protobuf::internal::kEmptyString) {
    detail_ = new ::std::string;
  }
  detail_->assign(value);
}
inline void MotionStateMessage::set_detail(const char* value, size_t size) {
  set_has_detail();
  if (detail_ == &::google::protobuf::internal::kEmptyString) {
    detail_ = new ::std::string;
  }
  detail_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* MotionStateMessage::mutable_detail() {
  set_has_detail();
  if (detail_ == &::google::protobuf::internal::kEmptyString) {
    detail_ = new ::std::string;
  }
  return detail_;
}
inline ::std::string* MotionStateMessage::release_detail() {
  clear_has_detail();
  if (detail_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = detail_;
    detail_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MotionStateMessage_ActionType>() {
  return ::MotionStateMessage_ActionType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_motion_2eproto__INCLUDED
