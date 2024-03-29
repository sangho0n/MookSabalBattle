// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: RegisterNickname.proto
#ifndef GRPC_RegisterNickname_2eproto__INCLUDED
#define GRPC_RegisterNickname_2eproto__INCLUDED

#include "RegisterNickname.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

class NicknameService final {
 public:
  static constexpr char const* service_full_name() {
    return "NicknameService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status RegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::RegisterResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::RegisterResponse>> AsyncRegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::RegisterResponse>>(AsyncRegisterNicknameRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::RegisterResponse>> PrepareAsyncRegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::RegisterResponse>>(PrepareAsyncRegisterNicknameRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void RegisterNickname(::grpc::ClientContext* context, const ::NickName* request, ::RegisterResponse* response, std::function<void(::grpc::Status)>) = 0;
      virtual void RegisterNickname(::grpc::ClientContext* context, const ::NickName* request, ::RegisterResponse* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::RegisterResponse>* AsyncRegisterNicknameRaw(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::RegisterResponse>* PrepareAsyncRegisterNicknameRaw(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status RegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::RegisterResponse* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::RegisterResponse>> AsyncRegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::RegisterResponse>>(AsyncRegisterNicknameRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::RegisterResponse>> PrepareAsyncRegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::RegisterResponse>>(PrepareAsyncRegisterNicknameRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void RegisterNickname(::grpc::ClientContext* context, const ::NickName* request, ::RegisterResponse* response, std::function<void(::grpc::Status)>) override;
      void RegisterNickname(::grpc::ClientContext* context, const ::NickName* request, ::RegisterResponse* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::RegisterResponse>* AsyncRegisterNicknameRaw(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::RegisterResponse>* PrepareAsyncRegisterNicknameRaw(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_RegisterNickname_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status RegisterNickname(::grpc::ServerContext* context, const ::NickName* request, ::RegisterResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_RegisterNickname : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_RegisterNickname() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_RegisterNickname() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status RegisterNickname(::grpc::ServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestRegisterNickname(::grpc::ServerContext* context, ::NickName* request, ::grpc::ServerAsyncResponseWriter< ::RegisterResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_RegisterNickname<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_RegisterNickname : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_RegisterNickname() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::NickName, ::RegisterResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::NickName* request, ::RegisterResponse* response) { return this->RegisterNickname(context, request, response); }));}
    void SetMessageAllocatorFor_RegisterNickname(
        ::grpc::MessageAllocator< ::NickName, ::RegisterResponse>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::NickName, ::RegisterResponse>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_RegisterNickname() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status RegisterNickname(::grpc::ServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* RegisterNickname(
      ::grpc::CallbackServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_RegisterNickname<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_RegisterNickname : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_RegisterNickname() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_RegisterNickname() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status RegisterNickname(::grpc::ServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_RegisterNickname : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_RegisterNickname() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_RegisterNickname() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status RegisterNickname(::grpc::ServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestRegisterNickname(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_RegisterNickname : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_RegisterNickname() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->RegisterNickname(context, request, response); }));
    }
    ~WithRawCallbackMethod_RegisterNickname() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status RegisterNickname(::grpc::ServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* RegisterNickname(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_RegisterNickname : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_RegisterNickname() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::NickName, ::RegisterResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::NickName, ::RegisterResponse>* streamer) {
                       return this->StreamedRegisterNickname(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_RegisterNickname() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status RegisterNickname(::grpc::ServerContext* /*context*/, const ::NickName* /*request*/, ::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedRegisterNickname(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::NickName,::RegisterResponse>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_RegisterNickname<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_RegisterNickname<Service > StreamedService;
};


#endif  // GRPC_RegisterNickname_2eproto__INCLUDED
