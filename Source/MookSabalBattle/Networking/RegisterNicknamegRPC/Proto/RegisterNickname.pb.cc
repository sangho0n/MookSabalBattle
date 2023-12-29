// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RegisterNickname.proto

#include "RegisterNickname.pb.h"
#pragma warning(disable:4800)

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

PROTOBUF_CONSTEXPR NickName::NickName(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.value_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct NickNameDefaultTypeInternal {
  PROTOBUF_CONSTEXPR NickNameDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~NickNameDefaultTypeInternal() {}
  union {
    NickName _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 NickNameDefaultTypeInternal _NickName_default_instance_;
PROTOBUF_CONSTEXPR RegisterResponse::RegisterResponse(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.bsucceed_)*/false
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct RegisterResponseDefaultTypeInternal {
  PROTOBUF_CONSTEXPR RegisterResponseDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~RegisterResponseDefaultTypeInternal() {}
  union {
    RegisterResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 RegisterResponseDefaultTypeInternal _RegisterResponse_default_instance_;
static ::_pb::Metadata file_level_metadata_RegisterNickname_2eproto[2];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_RegisterNickname_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_RegisterNickname_2eproto = nullptr;

const uint32_t TableStruct_RegisterNickname_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::NickName, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::NickName, _impl_.value_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::RegisterResponse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::RegisterResponse, _impl_.bsucceed_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::NickName)},
  { 7, -1, -1, sizeof(::RegisterResponse)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::_NickName_default_instance_._instance,
  &::_RegisterResponse_default_instance_._instance,
};

const char descriptor_table_protodef_RegisterNickname_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\026RegisterNickname.proto\"\031\n\010NickName\022\r\n\005"
  "value\030\001 \001(\t\"$\n\020RegisterResponse\022\020\n\010bSucc"
  "eed\030\001 \001(\0102C\n\017NicknameService\0220\n\020Register"
  "Nickname\022\t.NickName\032\021.RegisterResponseb\006"
  "proto3"
  ;
static ::_pbi::once_flag descriptor_table_RegisterNickname_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_RegisterNickname_2eproto = {
    false, false, 166, descriptor_table_protodef_RegisterNickname_2eproto,
    "RegisterNickname.proto",
    &descriptor_table_RegisterNickname_2eproto_once, nullptr, 0, 2,
    schemas, file_default_instances, TableStruct_RegisterNickname_2eproto::offsets,
    file_level_metadata_RegisterNickname_2eproto, file_level_enum_descriptors_RegisterNickname_2eproto,
    file_level_service_descriptors_RegisterNickname_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_RegisterNickname_2eproto_getter() {
  return &descriptor_table_RegisterNickname_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_RegisterNickname_2eproto(&descriptor_table_RegisterNickname_2eproto);

// ===================================================================

class NickName::_Internal {
 public:
};

NickName::NickName(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:NickName)
}
NickName::NickName(const NickName& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  NickName* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.value_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.value_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.value_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_value().empty()) {
    _this->_impl_.value_.Set(from._internal_value(), 
      _this->GetArenaForAllocation());
  }
  // @@protoc_insertion_point(copy_constructor:NickName)
}

inline void NickName::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.value_){}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.value_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.value_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

NickName::~NickName() {
  // @@protoc_insertion_point(destructor:NickName)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void NickName::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.value_.Destroy();
}

void NickName::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void NickName::Clear() {
// @@protoc_insertion_point(message_clear_start:NickName)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.value_.ClearToEmpty();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* NickName::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string value = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_value();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "NickName.value"));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* NickName::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:NickName)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string value = 1;
  if (!this->_internal_value().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_value().data(), static_cast<int>(this->_internal_value().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "NickName.value");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_value(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:NickName)
  return target;
}

size_t NickName::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:NickName)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string value = 1;
  if (!this->_internal_value().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_value());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData NickName::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    NickName::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*NickName::GetClassData() const { return &_class_data_; }


void NickName::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<NickName*>(&to_msg);
  auto& from = static_cast<const NickName&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:NickName)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_value().empty()) {
    _this->_internal_set_value(from._internal_value());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void NickName::CopyFrom(const NickName& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:NickName)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool NickName::IsInitialized() const {
  return true;
}

void NickName::InternalSwap(NickName* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.value_, lhs_arena,
      &other->_impl_.value_, rhs_arena
  );
}

::PROTOBUF_NAMESPACE_ID::Metadata NickName::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_RegisterNickname_2eproto_getter, &descriptor_table_RegisterNickname_2eproto_once,
      file_level_metadata_RegisterNickname_2eproto[0]);
}

// ===================================================================

class RegisterResponse::_Internal {
 public:
};

RegisterResponse::RegisterResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:RegisterResponse)
}
RegisterResponse::RegisterResponse(const RegisterResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  RegisterResponse* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.bsucceed_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _this->_impl_.bsucceed_ = from._impl_.bsucceed_;
  // @@protoc_insertion_point(copy_constructor:RegisterResponse)
}

inline void RegisterResponse::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.bsucceed_){false}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

RegisterResponse::~RegisterResponse() {
  // @@protoc_insertion_point(destructor:RegisterResponse)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void RegisterResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void RegisterResponse::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void RegisterResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:RegisterResponse)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.bsucceed_ = false;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RegisterResponse::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // bool bSucceed = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _impl_.bsucceed_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* RegisterResponse::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:RegisterResponse)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // bool bSucceed = 1;
  if (this->_internal_bsucceed() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteBoolToArray(1, this->_internal_bsucceed(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:RegisterResponse)
  return target;
}

size_t RegisterResponse::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:RegisterResponse)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bool bSucceed = 1;
  if (this->_internal_bsucceed() != 0) {
    total_size += 1 + 1;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData RegisterResponse::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    RegisterResponse::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*RegisterResponse::GetClassData() const { return &_class_data_; }


void RegisterResponse::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<RegisterResponse*>(&to_msg);
  auto& from = static_cast<const RegisterResponse&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:RegisterResponse)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_bsucceed() != 0) {
    _this->_internal_set_bsucceed(from._internal_bsucceed());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void RegisterResponse::CopyFrom(const RegisterResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:RegisterResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RegisterResponse::IsInitialized() const {
  return true;
}

void RegisterResponse::InternalSwap(RegisterResponse* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_.bsucceed_, other->_impl_.bsucceed_);
}

::PROTOBUF_NAMESPACE_ID::Metadata RegisterResponse::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_RegisterNickname_2eproto_getter, &descriptor_table_RegisterNickname_2eproto_once,
      file_level_metadata_RegisterNickname_2eproto[1]);
}

// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::NickName*
Arena::CreateMaybeMessage< ::NickName >(Arena* arena) {
  return Arena::CreateMessageInternal< ::NickName >(arena);
}
template<> PROTOBUF_NOINLINE ::RegisterResponse*
Arena::CreateMaybeMessage< ::RegisterResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::RegisterResponse >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
