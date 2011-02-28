// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BehaviorMessages.proto

#ifndef PROTOBUF_BehaviorMessages_2eproto__INCLUDED
#define PROTOBUF_BehaviorMessages_2eproto__INCLUDED

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
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_BehaviorMessages_2eproto();
void protobuf_AssignDesc_BehaviorMessages_2eproto();
void protobuf_ShutdownFile_BehaviorMessages_2eproto();

class KickOffMessage;
class BToHeadMessage;
class HeadToBMessage;
class ScanMessage;
class StateEnd;
class PlayerNumberMessage;

// ===================================================================

class KickOffMessage : public ::google::protobuf::Message {
 public:
  KickOffMessage();
  virtual ~KickOffMessage();
  
  KickOffMessage(const KickOffMessage& from);
  
  inline KickOffMessage& operator=(const KickOffMessage& from) {
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
  static const KickOffMessage& default_instance();
  
  void Swap(KickOffMessage* other);
  
  // implements Message ----------------------------------------------
  
  KickOffMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const KickOffMessage& from);
  void MergeFrom(const KickOffMessage& from);
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
  
  // required bool kickOff = 1 [default = false];
  inline bool has_kickoff() const;
  inline void clear_kickoff();
  static const int kKickOffFieldNumber = 1;
  inline bool kickoff() const;
  inline void set_kickoff(bool value);
  
  // @@protoc_insertion_point(class_scope:KickOffMessage)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  bool kickoff_;
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
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
  static KickOffMessage* default_instance_;
};
// -------------------------------------------------------------------

class BToHeadMessage : public ::google::protobuf::Message {
 public:
  BToHeadMessage();
  virtual ~BToHeadMessage();
  
  BToHeadMessage(const BToHeadMessage& from);
  
  inline BToHeadMessage& operator=(const BToHeadMessage& from) {
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
  static const BToHeadMessage& default_instance();
  
  void Swap(BToHeadMessage* other);
  
  // implements Message ----------------------------------------------
  
  BToHeadMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const BToHeadMessage& from);
  void MergeFrom(const BToHeadMessage& from);
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
  
  // required int32 headaction = 1 [default = 0];
  inline bool has_headaction() const;
  inline void clear_headaction();
  static const int kHeadactionFieldNumber = 1;
  inline ::google::protobuf::int32 headaction() const;
  inline void set_headaction(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:BToHeadMessage)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 headaction_;
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
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
  static BToHeadMessage* default_instance_;
};
// -------------------------------------------------------------------

class HeadToBMessage : public ::google::protobuf::Message {
 public:
  HeadToBMessage();
  virtual ~HeadToBMessage();
  
  HeadToBMessage(const HeadToBMessage& from);
  
  inline HeadToBMessage& operator=(const HeadToBMessage& from) {
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
  static const HeadToBMessage& default_instance();
  
  void Swap(HeadToBMessage* other);
  
  // implements Message ----------------------------------------------
  
  HeadToBMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const HeadToBMessage& from);
  void MergeFrom(const HeadToBMessage& from);
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
  
  // required int32 ballfound = 1 [default = 0];
  inline bool has_ballfound() const;
  inline void clear_ballfound();
  static const int kBallfoundFieldNumber = 1;
  inline ::google::protobuf::int32 ballfound() const;
  inline void set_ballfound(::google::protobuf::int32 value);
  
  // required int32 calibrated = 2 [default = 0];
  inline bool has_calibrated() const;
  inline void clear_calibrated();
  static const int kCalibratedFieldNumber = 2;
  inline ::google::protobuf::int32 calibrated() const;
  inline void set_calibrated(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:HeadToBMessage)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 ballfound_;
  ::google::protobuf::int32 calibrated_;
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
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
  static HeadToBMessage* default_instance_;
};
// -------------------------------------------------------------------

class ScanMessage : public ::google::protobuf::Message {
 public:
  ScanMessage();
  virtual ~ScanMessage();
  
  ScanMessage(const ScanMessage& from);
  
  inline ScanMessage& operator=(const ScanMessage& from) {
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
  static const ScanMessage& default_instance();
  
  void Swap(ScanMessage* other);
  
  // implements Message ----------------------------------------------
  
  ScanMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ScanMessage& from);
  void MergeFrom(const ScanMessage& from);
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
  
  // required bool scancompleted = 1 [default = false];
  inline bool has_scancompleted() const;
  inline void clear_scancompleted();
  static const int kScancompletedFieldNumber = 1;
  inline bool scancompleted() const;
  inline void set_scancompleted(bool value);
  
  // @@protoc_insertion_point(class_scope:ScanMessage)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  bool scancompleted_;
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
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
  static ScanMessage* default_instance_;
};
// -------------------------------------------------------------------

class StateEnd : public ::google::protobuf::Message {
 public:
  StateEnd();
  virtual ~StateEnd();
  
  StateEnd(const StateEnd& from);
  
  inline StateEnd& operator=(const StateEnd& from) {
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
  static const StateEnd& default_instance();
  
  void Swap(StateEnd* other);
  
  // implements Message ----------------------------------------------
  
  StateEnd* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StateEnd& from);
  void MergeFrom(const StateEnd& from);
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
  
  // required bool ended = 1 [default = false];
  inline bool has_ended() const;
  inline void clear_ended();
  static const int kEndedFieldNumber = 1;
  inline bool ended() const;
  inline void set_ended(bool value);
  
  // @@protoc_insertion_point(class_scope:StateEnd)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  bool ended_;
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
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
  static StateEnd* default_instance_;
};
// -------------------------------------------------------------------

class PlayerNumberMessage : public ::google::protobuf::Message {
 public:
  PlayerNumberMessage();
  virtual ~PlayerNumberMessage();
  
  PlayerNumberMessage(const PlayerNumberMessage& from);
  
  inline PlayerNumberMessage& operator=(const PlayerNumberMessage& from) {
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
  static const PlayerNumberMessage& default_instance();
  
  void Swap(PlayerNumberMessage* other);
  
  // implements Message ----------------------------------------------
  
  PlayerNumberMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PlayerNumberMessage& from);
  void MergeFrom(const PlayerNumberMessage& from);
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
  
  // required int32 player_number = 1 [default = 2];
  inline bool has_player_number() const;
  inline void clear_player_number();
  static const int kPlayerNumberFieldNumber = 1;
  inline ::google::protobuf::int32 player_number() const;
  inline void set_player_number(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:PlayerNumberMessage)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 player_number_;
  friend void  protobuf_AddDesc_BehaviorMessages_2eproto();
  friend void protobuf_AssignDesc_BehaviorMessages_2eproto();
  friend void protobuf_ShutdownFile_BehaviorMessages_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
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
  static PlayerNumberMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// KickOffMessage

// required bool kickOff = 1 [default = false];
inline bool KickOffMessage::has_kickoff() const {
  return _has_bit(0);
}
inline void KickOffMessage::clear_kickoff() {
  kickoff_ = false;
  _clear_bit(0);
}
inline bool KickOffMessage::kickoff() const {
  return kickoff_;
}
inline void KickOffMessage::set_kickoff(bool value) {
  _set_bit(0);
  kickoff_ = value;
}

// -------------------------------------------------------------------

// BToHeadMessage

// required int32 headaction = 1 [default = 0];
inline bool BToHeadMessage::has_headaction() const {
  return _has_bit(0);
}
inline void BToHeadMessage::clear_headaction() {
  headaction_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 BToHeadMessage::headaction() const {
  return headaction_;
}
inline void BToHeadMessage::set_headaction(::google::protobuf::int32 value) {
  _set_bit(0);
  headaction_ = value;
}

// -------------------------------------------------------------------

// HeadToBMessage

// required int32 ballfound = 1 [default = 0];
inline bool HeadToBMessage::has_ballfound() const {
  return _has_bit(0);
}
inline void HeadToBMessage::clear_ballfound() {
  ballfound_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 HeadToBMessage::ballfound() const {
  return ballfound_;
}
inline void HeadToBMessage::set_ballfound(::google::protobuf::int32 value) {
  _set_bit(0);
  ballfound_ = value;
}

// required int32 calibrated = 2 [default = 0];
inline bool HeadToBMessage::has_calibrated() const {
  return _has_bit(1);
}
inline void HeadToBMessage::clear_calibrated() {
  calibrated_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 HeadToBMessage::calibrated() const {
  return calibrated_;
}
inline void HeadToBMessage::set_calibrated(::google::protobuf::int32 value) {
  _set_bit(1);
  calibrated_ = value;
}

// -------------------------------------------------------------------

// ScanMessage

// required bool scancompleted = 1 [default = false];
inline bool ScanMessage::has_scancompleted() const {
  return _has_bit(0);
}
inline void ScanMessage::clear_scancompleted() {
  scancompleted_ = false;
  _clear_bit(0);
}
inline bool ScanMessage::scancompleted() const {
  return scancompleted_;
}
inline void ScanMessage::set_scancompleted(bool value) {
  _set_bit(0);
  scancompleted_ = value;
}

// -------------------------------------------------------------------

// StateEnd

// required bool ended = 1 [default = false];
inline bool StateEnd::has_ended() const {
  return _has_bit(0);
}
inline void StateEnd::clear_ended() {
  ended_ = false;
  _clear_bit(0);
}
inline bool StateEnd::ended() const {
  return ended_;
}
inline void StateEnd::set_ended(bool value) {
  _set_bit(0);
  ended_ = value;
}

// -------------------------------------------------------------------

// PlayerNumberMessage

// required int32 player_number = 1 [default = 2];
inline bool PlayerNumberMessage::has_player_number() const {
  return _has_bit(0);
}
inline void PlayerNumberMessage::clear_player_number() {
  player_number_ = 2;
  _clear_bit(0);
}
inline ::google::protobuf::int32 PlayerNumberMessage::player_number() const {
  return player_number_;
}
inline void PlayerNumberMessage::set_player_number(::google::protobuf::int32 value) {
  _set_bit(0);
  player_number_ = value;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_BehaviorMessages_2eproto__INCLUDED
