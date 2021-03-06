// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "timeout.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* TimeoutMsg_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  TimeoutMsg_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_timeout_2eproto() {
  protobuf_AddDesc_timeout_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "timeout.proto");
  GOOGLE_CHECK(file != NULL);
  TimeoutMsg_descriptor_ = file->message_type(0);
  static const int TimeoutMsg_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeoutMsg, wakeup_),
  };
  TimeoutMsg_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      TimeoutMsg_descriptor_,
      TimeoutMsg::default_instance_,
      TimeoutMsg_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeoutMsg, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeoutMsg, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(TimeoutMsg));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_timeout_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    TimeoutMsg_descriptor_, &TimeoutMsg::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_timeout_2eproto() {
  delete TimeoutMsg::default_instance_;
  delete TimeoutMsg_reflection_;
}

void protobuf_AddDesc_timeout_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\rtimeout.proto\"\034\n\nTimeoutMsg\022\016\n\006wakeup\030"
    "\001 \002(\t", 45);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "timeout.proto", &protobuf_RegisterTypes);
  TimeoutMsg::default_instance_ = new TimeoutMsg();
  TimeoutMsg::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_timeout_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_timeout_2eproto {
  StaticDescriptorInitializer_timeout_2eproto() {
    protobuf_AddDesc_timeout_2eproto();
  }
} static_descriptor_initializer_timeout_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int TimeoutMsg::kWakeupFieldNumber;
#endif  // !_MSC_VER

TimeoutMsg::TimeoutMsg()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void TimeoutMsg::InitAsDefaultInstance() {
}

TimeoutMsg::TimeoutMsg(const TimeoutMsg& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void TimeoutMsg::SharedCtor() {
  _cached_size_ = 0;
  wakeup_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

TimeoutMsg::~TimeoutMsg() {
  SharedDtor();
}

void TimeoutMsg::SharedDtor() {
  if (wakeup_ != &::google::protobuf::internal::kEmptyString) {
    delete wakeup_;
  }
  if (this != default_instance_) {
  }
}

void TimeoutMsg::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TimeoutMsg::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return TimeoutMsg_descriptor_;
}

const TimeoutMsg& TimeoutMsg::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_timeout_2eproto();  return *default_instance_;
}

TimeoutMsg* TimeoutMsg::default_instance_ = NULL;

TimeoutMsg* TimeoutMsg::New() const {
  return new TimeoutMsg;
}

void TimeoutMsg::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_wakeup()) {
      if (wakeup_ != &::google::protobuf::internal::kEmptyString) {
        wakeup_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool TimeoutMsg::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string wakeup = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_wakeup()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->wakeup().data(), this->wakeup().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void TimeoutMsg::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string wakeup = 1;
  if (has_wakeup()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->wakeup().data(), this->wakeup().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->wakeup(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* TimeoutMsg::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string wakeup = 1;
  if (has_wakeup()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->wakeup().data(), this->wakeup().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->wakeup(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int TimeoutMsg::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string wakeup = 1;
    if (has_wakeup()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->wakeup());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void TimeoutMsg::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const TimeoutMsg* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const TimeoutMsg*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void TimeoutMsg::MergeFrom(const TimeoutMsg& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_wakeup()) {
      set_wakeup(from.wakeup());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void TimeoutMsg::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TimeoutMsg::CopyFrom(const TimeoutMsg& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TimeoutMsg::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void TimeoutMsg::Swap(TimeoutMsg* other) {
  if (other != this) {
    std::swap(wakeup_, other->wakeup_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata TimeoutMsg::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = TimeoutMsg_descriptor_;
  metadata.reflection = TimeoutMsg_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
