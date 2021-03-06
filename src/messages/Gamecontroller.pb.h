// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Gamecontroller.proto

#ifndef PROTOBUF_Gamecontroller_2eproto__INCLUDED
#define PROTOBUF_Gamecontroller_2eproto__INCLUDED

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
void  protobuf_AddDesc_Gamecontroller_2eproto();
void protobuf_AssignDesc_Gamecontroller_2eproto();
void protobuf_ShutdownFile_Gamecontroller_2eproto();

class GameStateMessage;
class LedValues;
class LedChangeMessage;
class ConfigMessage;

// ===================================================================

class GameStateMessage : public ::google::protobuf::Message {
 public:
  GameStateMessage();
  virtual ~GameStateMessage();
  
  GameStateMessage(const GameStateMessage& from);
  
  inline GameStateMessage& operator=(const GameStateMessage& from) {
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
  static const GameStateMessage& default_instance();
  
  void Swap(GameStateMessage* other);
  
  // implements Message ----------------------------------------------
  
  GameStateMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GameStateMessage& from);
  void MergeFrom(const GameStateMessage& from);
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
  
  // required uint32 player_number = 6 [default = 1];
  inline bool has_player_number() const;
  inline void clear_player_number();
  static const int kPlayerNumberFieldNumber = 6;
  inline ::google::protobuf::uint32 player_number() const;
  inline void set_player_number(::google::protobuf::uint32 value);
  
  // required uint32 team_number = 1 [default = 9];
  inline bool has_team_number() const;
  inline void clear_team_number();
  static const int kTeamNumberFieldNumber = 1;
  inline ::google::protobuf::uint32 team_number() const;
  inline void set_team_number(::google::protobuf::uint32 value);
  
  // required uint32 team_color = 7 [default = 0];
  inline bool has_team_color() const;
  inline void clear_team_color();
  static const int kTeamColorFieldNumber = 7;
  inline ::google::protobuf::uint32 team_color() const;
  inline void set_team_color(::google::protobuf::uint32 value);
  
  // required uint32 own_goal_color = 5 [default = 0];
  inline bool has_own_goal_color() const;
  inline void clear_own_goal_color();
  static const int kOwnGoalColorFieldNumber = 5;
  inline ::google::protobuf::uint32 own_goal_color() const;
  inline void set_own_goal_color(::google::protobuf::uint32 value);
  
  // required uint32 player_state = 8 [default = 0];
  inline bool has_player_state() const;
  inline void clear_player_state();
  static const int kPlayerStateFieldNumber = 8;
  inline ::google::protobuf::uint32 player_state() const;
  inline void set_player_state(::google::protobuf::uint32 value);
  
  // required uint32 game_state = 9 [default = 0];
  inline bool has_game_state() const;
  inline void clear_game_state();
  static const int kGameStateFieldNumber = 9;
  inline ::google::protobuf::uint32 game_state() const;
  inline void set_game_state(::google::protobuf::uint32 value);
  
  // required uint32 sec_game_state = 10 [default = 0];
  inline bool has_sec_game_state() const;
  inline void clear_sec_game_state();
  static const int kSecGameStateFieldNumber = 10;
  inline ::google::protobuf::uint32 sec_game_state() const;
  inline void set_sec_game_state(::google::protobuf::uint32 value);
  
  // required bool firstHalf = 3 [default = true];
  inline bool has_firsthalf() const;
  inline void clear_firsthalf();
  static const int kFirstHalfFieldNumber = 3;
  inline bool firsthalf() const;
  inline void set_firsthalf(bool value);
  
  // required uint32 penalty = 13 [default = 0];
  inline bool has_penalty() const;
  inline void clear_penalty();
  static const int kPenaltyFieldNumber = 13;
  inline ::google::protobuf::uint32 penalty() const;
  inline void set_penalty(::google::protobuf::uint32 value);
  
  // required bool kickoff = 12 [default = true];
  inline bool has_kickoff() const;
  inline void clear_kickoff();
  static const int kKickoffFieldNumber = 12;
  inline bool kickoff() const;
  inline void set_kickoff(bool value);
  
  // required uint32 our_score = 14 [default = 0];
  inline bool has_our_score() const;
  inline void clear_our_score();
  static const int kOurScoreFieldNumber = 14;
  inline ::google::protobuf::uint32 our_score() const;
  inline void set_our_score(::google::protobuf::uint32 value);
  
  // required uint32 otherteam_score = 15 [default = 0];
  inline bool has_otherteam_score() const;
  inline void clear_otherteam_score();
  static const int kOtherteamScoreFieldNumber = 15;
  inline ::google::protobuf::uint32 otherteam_score() const;
  inline void set_otherteam_score(::google::protobuf::uint32 value);
  
  // required uint32 override_state = 16 [default = 0];
  inline bool has_override_state() const;
  inline void clear_override_state();
  static const int kOverrideStateFieldNumber = 16;
  inline ::google::protobuf::uint32 override_state() const;
  inline void set_override_state(::google::protobuf::uint32 value);
  
  // required uint32 previous_player_state = 17 [default = 0];
  inline bool has_previous_player_state() const;
  inline void clear_previous_player_state();
  static const int kPreviousPlayerStateFieldNumber = 17;
  inline ::google::protobuf::uint32 previous_player_state() const;
  inline void set_previous_player_state(::google::protobuf::uint32 value);
  
  // required uint32 secs_remaining = 2 [default = 0];
  inline bool has_secs_remaining() const;
  inline void clear_secs_remaining();
  static const int kSecsRemainingFieldNumber = 2;
  inline ::google::protobuf::uint32 secs_remaining() const;
  inline void set_secs_remaining(::google::protobuf::uint32 value);
  
  // @@protoc_insertion_point(class_scope:GameStateMessage)
 private:
  inline void set_has_player_number();
  inline void clear_has_player_number();
  inline void set_has_team_number();
  inline void clear_has_team_number();
  inline void set_has_team_color();
  inline void clear_has_team_color();
  inline void set_has_own_goal_color();
  inline void clear_has_own_goal_color();
  inline void set_has_player_state();
  inline void clear_has_player_state();
  inline void set_has_game_state();
  inline void clear_has_game_state();
  inline void set_has_sec_game_state();
  inline void clear_has_sec_game_state();
  inline void set_has_firsthalf();
  inline void clear_has_firsthalf();
  inline void set_has_penalty();
  inline void clear_has_penalty();
  inline void set_has_kickoff();
  inline void clear_has_kickoff();
  inline void set_has_our_score();
  inline void clear_has_our_score();
  inline void set_has_otherteam_score();
  inline void clear_has_otherteam_score();
  inline void set_has_override_state();
  inline void clear_has_override_state();
  inline void set_has_previous_player_state();
  inline void clear_has_previous_player_state();
  inline void set_has_secs_remaining();
  inline void clear_has_secs_remaining();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::uint32 player_number_;
  ::google::protobuf::uint32 team_number_;
  ::google::protobuf::uint32 team_color_;
  ::google::protobuf::uint32 own_goal_color_;
  ::google::protobuf::uint32 player_state_;
  ::google::protobuf::uint32 game_state_;
  ::google::protobuf::uint32 sec_game_state_;
  ::google::protobuf::uint32 penalty_;
  bool firsthalf_;
  bool kickoff_;
  ::google::protobuf::uint32 our_score_;
  ::google::protobuf::uint32 otherteam_score_;
  ::google::protobuf::uint32 override_state_;
  ::google::protobuf::uint32 previous_player_state_;
  ::google::protobuf::uint32 secs_remaining_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(15 + 31) / 32];
  
  friend void  protobuf_AddDesc_Gamecontroller_2eproto();
  friend void protobuf_AssignDesc_Gamecontroller_2eproto();
  friend void protobuf_ShutdownFile_Gamecontroller_2eproto();
  
  void InitAsDefaultInstance();
  static GameStateMessage* default_instance_;
};
// -------------------------------------------------------------------

class LedValues : public ::google::protobuf::Message {
 public:
  LedValues();
  virtual ~LedValues();
  
  LedValues(const LedValues& from);
  
  inline LedValues& operator=(const LedValues& from) {
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
  static const LedValues& default_instance();
  
  void Swap(LedValues* other);
  
  // implements Message ----------------------------------------------
  
  LedValues* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LedValues& from);
  void MergeFrom(const LedValues& from);
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
  
  // required string chain = 1 [default = ""];
  inline bool has_chain() const;
  inline void clear_chain();
  static const int kChainFieldNumber = 1;
  inline const ::std::string& chain() const;
  inline void set_chain(const ::std::string& value);
  inline void set_chain(const char* value);
  inline void set_chain(const char* value, size_t size);
  inline ::std::string* mutable_chain();
  inline ::std::string* release_chain();
  
  // required string color = 2 [default = "off"];
  inline bool has_color() const;
  inline void clear_color();
  static const int kColorFieldNumber = 2;
  inline const ::std::string& color() const;
  inline void set_color(const ::std::string& value);
  inline void set_color(const char* value);
  inline void set_color(const char* value, size_t size);
  inline ::std::string* mutable_color();
  inline ::std::string* release_color();
  
  // @@protoc_insertion_point(class_scope:LedValues)
 private:
  inline void set_has_chain();
  inline void clear_has_chain();
  inline void set_has_color();
  inline void clear_has_color();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* chain_;
  ::std::string* color_;
  static const ::std::string _default_color_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_Gamecontroller_2eproto();
  friend void protobuf_AssignDesc_Gamecontroller_2eproto();
  friend void protobuf_ShutdownFile_Gamecontroller_2eproto();
  
  void InitAsDefaultInstance();
  static LedValues* default_instance_;
};
// -------------------------------------------------------------------

class LedChangeMessage : public ::google::protobuf::Message {
 public:
  LedChangeMessage();
  virtual ~LedChangeMessage();
  
  LedChangeMessage(const LedChangeMessage& from);
  
  inline LedChangeMessage& operator=(const LedChangeMessage& from) {
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
  static const LedChangeMessage& default_instance();
  
  void Swap(LedChangeMessage* other);
  
  // implements Message ----------------------------------------------
  
  LedChangeMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LedChangeMessage& from);
  void MergeFrom(const LedChangeMessage& from);
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
  
  // repeated .LedValues leds = 6;
  inline int leds_size() const;
  inline void clear_leds();
  static const int kLedsFieldNumber = 6;
  inline const ::LedValues& leds(int index) const;
  inline ::LedValues* mutable_leds(int index);
  inline ::LedValues* add_leds();
  inline const ::google::protobuf::RepeatedPtrField< ::LedValues >&
      leds() const;
  inline ::google::protobuf::RepeatedPtrField< ::LedValues >*
      mutable_leds();
  
  // @@protoc_insertion_point(class_scope:LedChangeMessage)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::LedValues > leds_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_Gamecontroller_2eproto();
  friend void protobuf_AssignDesc_Gamecontroller_2eproto();
  friend void protobuf_ShutdownFile_Gamecontroller_2eproto();
  
  void InitAsDefaultInstance();
  static LedChangeMessage* default_instance_;
};
// -------------------------------------------------------------------

class ConfigMessage : public ::google::protobuf::Message {
 public:
  ConfigMessage();
  virtual ~ConfigMessage();
  
  ConfigMessage(const ConfigMessage& from);
  
  inline ConfigMessage& operator=(const ConfigMessage& from) {
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
  static const ConfigMessage& default_instance();
  
  void Swap(ConfigMessage* other);
  
  // implements Message ----------------------------------------------
  
  ConfigMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ConfigMessage& from);
  void MergeFrom(const ConfigMessage& from);
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
  
  // required int32 team_number = 1 [default = 9];
  inline bool has_team_number() const;
  inline void clear_team_number();
  static const int kTeamNumberFieldNumber = 1;
  inline ::google::protobuf::int32 team_number() const;
  inline void set_team_number(::google::protobuf::int32 value);
  
  // required int32 player_number = 2 [default = 1];
  inline bool has_player_number() const;
  inline void clear_player_number();
  static const int kPlayerNumberFieldNumber = 2;
  inline ::google::protobuf::int32 player_number() const;
  inline void set_player_number(::google::protobuf::int32 value);
  
  // required int32 team_color = 4 [default = 0];
  inline bool has_team_color() const;
  inline void clear_team_color();
  static const int kTeamColorFieldNumber = 4;
  inline ::google::protobuf::int32 team_color() const;
  inline void set_team_color(::google::protobuf::int32 value);
  
  // required int32 port = 3 [default = 3838];
  inline bool has_port() const;
  inline void clear_port();
  static const int kPortFieldNumber = 3;
  inline ::google::protobuf::int32 port() const;
  inline void set_port(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:ConfigMessage)
 private:
  inline void set_has_team_number();
  inline void clear_has_team_number();
  inline void set_has_player_number();
  inline void clear_has_player_number();
  inline void set_has_team_color();
  inline void clear_has_team_color();
  inline void set_has_port();
  inline void clear_has_port();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::int32 team_number_;
  ::google::protobuf::int32 player_number_;
  ::google::protobuf::int32 team_color_;
  ::google::protobuf::int32 port_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_Gamecontroller_2eproto();
  friend void protobuf_AssignDesc_Gamecontroller_2eproto();
  friend void protobuf_ShutdownFile_Gamecontroller_2eproto();
  
  void InitAsDefaultInstance();
  static ConfigMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// GameStateMessage

// required uint32 player_number = 6 [default = 1];
inline bool GameStateMessage::has_player_number() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void GameStateMessage::set_has_player_number() {
  _has_bits_[0] |= 0x00000001u;
}
inline void GameStateMessage::clear_has_player_number() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void GameStateMessage::clear_player_number() {
  player_number_ = 1u;
  clear_has_player_number();
}
inline ::google::protobuf::uint32 GameStateMessage::player_number() const {
  return player_number_;
}
inline void GameStateMessage::set_player_number(::google::protobuf::uint32 value) {
  set_has_player_number();
  player_number_ = value;
}

// required uint32 team_number = 1 [default = 9];
inline bool GameStateMessage::has_team_number() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void GameStateMessage::set_has_team_number() {
  _has_bits_[0] |= 0x00000002u;
}
inline void GameStateMessage::clear_has_team_number() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void GameStateMessage::clear_team_number() {
  team_number_ = 9u;
  clear_has_team_number();
}
inline ::google::protobuf::uint32 GameStateMessage::team_number() const {
  return team_number_;
}
inline void GameStateMessage::set_team_number(::google::protobuf::uint32 value) {
  set_has_team_number();
  team_number_ = value;
}

// required uint32 team_color = 7 [default = 0];
inline bool GameStateMessage::has_team_color() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void GameStateMessage::set_has_team_color() {
  _has_bits_[0] |= 0x00000004u;
}
inline void GameStateMessage::clear_has_team_color() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void GameStateMessage::clear_team_color() {
  team_color_ = 0u;
  clear_has_team_color();
}
inline ::google::protobuf::uint32 GameStateMessage::team_color() const {
  return team_color_;
}
inline void GameStateMessage::set_team_color(::google::protobuf::uint32 value) {
  set_has_team_color();
  team_color_ = value;
}

// required uint32 own_goal_color = 5 [default = 0];
inline bool GameStateMessage::has_own_goal_color() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void GameStateMessage::set_has_own_goal_color() {
  _has_bits_[0] |= 0x00000008u;
}
inline void GameStateMessage::clear_has_own_goal_color() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void GameStateMessage::clear_own_goal_color() {
  own_goal_color_ = 0u;
  clear_has_own_goal_color();
}
inline ::google::protobuf::uint32 GameStateMessage::own_goal_color() const {
  return own_goal_color_;
}
inline void GameStateMessage::set_own_goal_color(::google::protobuf::uint32 value) {
  set_has_own_goal_color();
  own_goal_color_ = value;
}

// required uint32 player_state = 8 [default = 0];
inline bool GameStateMessage::has_player_state() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void GameStateMessage::set_has_player_state() {
  _has_bits_[0] |= 0x00000010u;
}
inline void GameStateMessage::clear_has_player_state() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void GameStateMessage::clear_player_state() {
  player_state_ = 0u;
  clear_has_player_state();
}
inline ::google::protobuf::uint32 GameStateMessage::player_state() const {
  return player_state_;
}
inline void GameStateMessage::set_player_state(::google::protobuf::uint32 value) {
  set_has_player_state();
  player_state_ = value;
}

// required uint32 game_state = 9 [default = 0];
inline bool GameStateMessage::has_game_state() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void GameStateMessage::set_has_game_state() {
  _has_bits_[0] |= 0x00000020u;
}
inline void GameStateMessage::clear_has_game_state() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void GameStateMessage::clear_game_state() {
  game_state_ = 0u;
  clear_has_game_state();
}
inline ::google::protobuf::uint32 GameStateMessage::game_state() const {
  return game_state_;
}
inline void GameStateMessage::set_game_state(::google::protobuf::uint32 value) {
  set_has_game_state();
  game_state_ = value;
}

// required uint32 sec_game_state = 10 [default = 0];
inline bool GameStateMessage::has_sec_game_state() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void GameStateMessage::set_has_sec_game_state() {
  _has_bits_[0] |= 0x00000040u;
}
inline void GameStateMessage::clear_has_sec_game_state() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void GameStateMessage::clear_sec_game_state() {
  sec_game_state_ = 0u;
  clear_has_sec_game_state();
}
inline ::google::protobuf::uint32 GameStateMessage::sec_game_state() const {
  return sec_game_state_;
}
inline void GameStateMessage::set_sec_game_state(::google::protobuf::uint32 value) {
  set_has_sec_game_state();
  sec_game_state_ = value;
}

// required bool firstHalf = 3 [default = true];
inline bool GameStateMessage::has_firsthalf() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void GameStateMessage::set_has_firsthalf() {
  _has_bits_[0] |= 0x00000080u;
}
inline void GameStateMessage::clear_has_firsthalf() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void GameStateMessage::clear_firsthalf() {
  firsthalf_ = true;
  clear_has_firsthalf();
}
inline bool GameStateMessage::firsthalf() const {
  return firsthalf_;
}
inline void GameStateMessage::set_firsthalf(bool value) {
  set_has_firsthalf();
  firsthalf_ = value;
}

// required uint32 penalty = 13 [default = 0];
inline bool GameStateMessage::has_penalty() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void GameStateMessage::set_has_penalty() {
  _has_bits_[0] |= 0x00000100u;
}
inline void GameStateMessage::clear_has_penalty() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void GameStateMessage::clear_penalty() {
  penalty_ = 0u;
  clear_has_penalty();
}
inline ::google::protobuf::uint32 GameStateMessage::penalty() const {
  return penalty_;
}
inline void GameStateMessage::set_penalty(::google::protobuf::uint32 value) {
  set_has_penalty();
  penalty_ = value;
}

// required bool kickoff = 12 [default = true];
inline bool GameStateMessage::has_kickoff() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void GameStateMessage::set_has_kickoff() {
  _has_bits_[0] |= 0x00000200u;
}
inline void GameStateMessage::clear_has_kickoff() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void GameStateMessage::clear_kickoff() {
  kickoff_ = true;
  clear_has_kickoff();
}
inline bool GameStateMessage::kickoff() const {
  return kickoff_;
}
inline void GameStateMessage::set_kickoff(bool value) {
  set_has_kickoff();
  kickoff_ = value;
}

// required uint32 our_score = 14 [default = 0];
inline bool GameStateMessage::has_our_score() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void GameStateMessage::set_has_our_score() {
  _has_bits_[0] |= 0x00000400u;
}
inline void GameStateMessage::clear_has_our_score() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void GameStateMessage::clear_our_score() {
  our_score_ = 0u;
  clear_has_our_score();
}
inline ::google::protobuf::uint32 GameStateMessage::our_score() const {
  return our_score_;
}
inline void GameStateMessage::set_our_score(::google::protobuf::uint32 value) {
  set_has_our_score();
  our_score_ = value;
}

// required uint32 otherteam_score = 15 [default = 0];
inline bool GameStateMessage::has_otherteam_score() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void GameStateMessage::set_has_otherteam_score() {
  _has_bits_[0] |= 0x00000800u;
}
inline void GameStateMessage::clear_has_otherteam_score() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void GameStateMessage::clear_otherteam_score() {
  otherteam_score_ = 0u;
  clear_has_otherteam_score();
}
inline ::google::protobuf::uint32 GameStateMessage::otherteam_score() const {
  return otherteam_score_;
}
inline void GameStateMessage::set_otherteam_score(::google::protobuf::uint32 value) {
  set_has_otherteam_score();
  otherteam_score_ = value;
}

// required uint32 override_state = 16 [default = 0];
inline bool GameStateMessage::has_override_state() const {
  return (_has_bits_[0] & 0x00001000u) != 0;
}
inline void GameStateMessage::set_has_override_state() {
  _has_bits_[0] |= 0x00001000u;
}
inline void GameStateMessage::clear_has_override_state() {
  _has_bits_[0] &= ~0x00001000u;
}
inline void GameStateMessage::clear_override_state() {
  override_state_ = 0u;
  clear_has_override_state();
}
inline ::google::protobuf::uint32 GameStateMessage::override_state() const {
  return override_state_;
}
inline void GameStateMessage::set_override_state(::google::protobuf::uint32 value) {
  set_has_override_state();
  override_state_ = value;
}

// required uint32 previous_player_state = 17 [default = 0];
inline bool GameStateMessage::has_previous_player_state() const {
  return (_has_bits_[0] & 0x00002000u) != 0;
}
inline void GameStateMessage::set_has_previous_player_state() {
  _has_bits_[0] |= 0x00002000u;
}
inline void GameStateMessage::clear_has_previous_player_state() {
  _has_bits_[0] &= ~0x00002000u;
}
inline void GameStateMessage::clear_previous_player_state() {
  previous_player_state_ = 0u;
  clear_has_previous_player_state();
}
inline ::google::protobuf::uint32 GameStateMessage::previous_player_state() const {
  return previous_player_state_;
}
inline void GameStateMessage::set_previous_player_state(::google::protobuf::uint32 value) {
  set_has_previous_player_state();
  previous_player_state_ = value;
}

// required uint32 secs_remaining = 2 [default = 0];
inline bool GameStateMessage::has_secs_remaining() const {
  return (_has_bits_[0] & 0x00004000u) != 0;
}
inline void GameStateMessage::set_has_secs_remaining() {
  _has_bits_[0] |= 0x00004000u;
}
inline void GameStateMessage::clear_has_secs_remaining() {
  _has_bits_[0] &= ~0x00004000u;
}
inline void GameStateMessage::clear_secs_remaining() {
  secs_remaining_ = 0u;
  clear_has_secs_remaining();
}
inline ::google::protobuf::uint32 GameStateMessage::secs_remaining() const {
  return secs_remaining_;
}
inline void GameStateMessage::set_secs_remaining(::google::protobuf::uint32 value) {
  set_has_secs_remaining();
  secs_remaining_ = value;
}

// -------------------------------------------------------------------

// LedValues

// required string chain = 1 [default = ""];
inline bool LedValues::has_chain() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void LedValues::set_has_chain() {
  _has_bits_[0] |= 0x00000001u;
}
inline void LedValues::clear_has_chain() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void LedValues::clear_chain() {
  if (chain_ != &::google::protobuf::internal::kEmptyString) {
    chain_->clear();
  }
  clear_has_chain();
}
inline const ::std::string& LedValues::chain() const {
  return *chain_;
}
inline void LedValues::set_chain(const ::std::string& value) {
  set_has_chain();
  if (chain_ == &::google::protobuf::internal::kEmptyString) {
    chain_ = new ::std::string;
  }
  chain_->assign(value);
}
inline void LedValues::set_chain(const char* value) {
  set_has_chain();
  if (chain_ == &::google::protobuf::internal::kEmptyString) {
    chain_ = new ::std::string;
  }
  chain_->assign(value);
}
inline void LedValues::set_chain(const char* value, size_t size) {
  set_has_chain();
  if (chain_ == &::google::protobuf::internal::kEmptyString) {
    chain_ = new ::std::string;
  }
  chain_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* LedValues::mutable_chain() {
  set_has_chain();
  if (chain_ == &::google::protobuf::internal::kEmptyString) {
    chain_ = new ::std::string;
  }
  return chain_;
}
inline ::std::string* LedValues::release_chain() {
  clear_has_chain();
  if (chain_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = chain_;
    chain_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required string color = 2 [default = "off"];
inline bool LedValues::has_color() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void LedValues::set_has_color() {
  _has_bits_[0] |= 0x00000002u;
}
inline void LedValues::clear_has_color() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void LedValues::clear_color() {
  if (color_ != &_default_color_) {
    color_->assign(_default_color_);
  }
  clear_has_color();
}
inline const ::std::string& LedValues::color() const {
  return *color_;
}
inline void LedValues::set_color(const ::std::string& value) {
  set_has_color();
  if (color_ == &_default_color_) {
    color_ = new ::std::string;
  }
  color_->assign(value);
}
inline void LedValues::set_color(const char* value) {
  set_has_color();
  if (color_ == &_default_color_) {
    color_ = new ::std::string;
  }
  color_->assign(value);
}
inline void LedValues::set_color(const char* value, size_t size) {
  set_has_color();
  if (color_ == &_default_color_) {
    color_ = new ::std::string;
  }
  color_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* LedValues::mutable_color() {
  set_has_color();
  if (color_ == &_default_color_) {
    color_ = new ::std::string(_default_color_);
  }
  return color_;
}
inline ::std::string* LedValues::release_color() {
  clear_has_color();
  if (color_ == &_default_color_) {
    return NULL;
  } else {
    ::std::string* temp = color_;
    color_ = const_cast< ::std::string*>(&_default_color_);
    return temp;
  }
}

// -------------------------------------------------------------------

// LedChangeMessage

// repeated .LedValues leds = 6;
inline int LedChangeMessage::leds_size() const {
  return leds_.size();
}
inline void LedChangeMessage::clear_leds() {
  leds_.Clear();
}
inline const ::LedValues& LedChangeMessage::leds(int index) const {
  return leds_.Get(index);
}
inline ::LedValues* LedChangeMessage::mutable_leds(int index) {
  return leds_.Mutable(index);
}
inline ::LedValues* LedChangeMessage::add_leds() {
  return leds_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::LedValues >&
LedChangeMessage::leds() const {
  return leds_;
}
inline ::google::protobuf::RepeatedPtrField< ::LedValues >*
LedChangeMessage::mutable_leds() {
  return &leds_;
}

// -------------------------------------------------------------------

// ConfigMessage

// required int32 team_number = 1 [default = 9];
inline bool ConfigMessage::has_team_number() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ConfigMessage::set_has_team_number() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ConfigMessage::clear_has_team_number() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ConfigMessage::clear_team_number() {
  team_number_ = 9;
  clear_has_team_number();
}
inline ::google::protobuf::int32 ConfigMessage::team_number() const {
  return team_number_;
}
inline void ConfigMessage::set_team_number(::google::protobuf::int32 value) {
  set_has_team_number();
  team_number_ = value;
}

// required int32 player_number = 2 [default = 1];
inline bool ConfigMessage::has_player_number() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ConfigMessage::set_has_player_number() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ConfigMessage::clear_has_player_number() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ConfigMessage::clear_player_number() {
  player_number_ = 1;
  clear_has_player_number();
}
inline ::google::protobuf::int32 ConfigMessage::player_number() const {
  return player_number_;
}
inline void ConfigMessage::set_player_number(::google::protobuf::int32 value) {
  set_has_player_number();
  player_number_ = value;
}

// required int32 team_color = 4 [default = 0];
inline bool ConfigMessage::has_team_color() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ConfigMessage::set_has_team_color() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ConfigMessage::clear_has_team_color() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ConfigMessage::clear_team_color() {
  team_color_ = 0;
  clear_has_team_color();
}
inline ::google::protobuf::int32 ConfigMessage::team_color() const {
  return team_color_;
}
inline void ConfigMessage::set_team_color(::google::protobuf::int32 value) {
  set_has_team_color();
  team_color_ = value;
}

// required int32 port = 3 [default = 3838];
inline bool ConfigMessage::has_port() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ConfigMessage::set_has_port() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ConfigMessage::clear_has_port() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ConfigMessage::clear_port() {
  port_ = 3838;
  clear_has_port();
}
inline ::google::protobuf::int32 ConfigMessage::port() const {
  return port_;
}
inline void ConfigMessage::set_port(::google::protobuf::int32 value) {
  set_has_port();
  port_ = value;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Gamecontroller_2eproto__INCLUDED
