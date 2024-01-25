// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: RegisterNickname.proto

#include "RegisterNickname.pb.h"
#include "RegisterNickname.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>

static const char* NicknameService_method_names[] = {
  "/NicknameService/RegisterNickname",
};

std::unique_ptr< NicknameService::Stub> NicknameService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< NicknameService::Stub> stub(new NicknameService::Stub(channel, options));
  return stub;
}

NicknameService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_RegisterNickname_(NicknameService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status NicknameService::Stub::RegisterNickname(::grpc::ClientContext* context, const ::NickName& request, ::RegisterResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::NickName, ::RegisterResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_RegisterNickname_, context, request, response);
}

void NicknameService::Stub::async::RegisterNickname(::grpc::ClientContext* context, const ::NickName* request, ::RegisterResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::NickName, ::RegisterResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RegisterNickname_, context, request, response, std::move(f));
}

void NicknameService::Stub::async::RegisterNickname(::grpc::ClientContext* context, const ::NickName* request, ::RegisterResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RegisterNickname_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::RegisterResponse>* NicknameService::Stub::PrepareAsyncRegisterNicknameRaw(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::RegisterResponse, ::NickName, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_RegisterNickname_, context, request);
}

::grpc::ClientAsyncResponseReader< ::RegisterResponse>* NicknameService::Stub::AsyncRegisterNicknameRaw(::grpc::ClientContext* context, const ::NickName& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncRegisterNicknameRaw(context, request, cq);
  result->StartCall();
  return result;
}

NicknameService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      NicknameService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< NicknameService::Service, ::NickName, ::RegisterResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](NicknameService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::NickName* req,
             ::RegisterResponse* resp) {
               return service->RegisterNickname(ctx, req, resp);
             }, this)));
}

NicknameService::Service::~Service() {
}

::grpc::Status NicknameService::Service::RegisterNickname(::grpc::ServerContext* context, const ::NickName* request, ::RegisterResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


