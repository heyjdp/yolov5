/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// This is a modification of greeter_client.cc from grpc examples.
// Comments have been removed to make it easier to follow the code.
// For comments please refer to the original example.

// NOTE Found here: https://raw.githubusercontent.com/G-Research/grpc_async_examples/master/greeter_streaming_client.cc

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "yolostreamingoutput.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using yolostreamingoutput::YoloReply;
using yolostreamingoutput::YoloRequest;
using yolostreamingoutput::YoloStreamer;

class YoloClient
{
public:
    YoloClient(std::shared_ptr<Channel> channel)
        : stub_(YoloStreamer::NewStub(channel))
    {
    }

    void SayHello(const std::string &message)
    {
        YoloRequest request;
        request.set_message(message);

        std::cout << "Hello2" << std::endl;

        ClientContext context;
        std::unique_ptr<ClientReader<YoloReply>> reader(stub_->yoloDetection(&context, request));

        YoloReply reply;
        while (reader->Read(&reply))
        {
            std::cout << "Got reply: " << reply.detection() << std::endl;
        }

        Status status = reader->Finish();
        if (status.ok())
        {
            std::cout << "yoloDetection rpc succeeded." << std::endl;
        }
        else
        {
            std::cout << "yoloDetection rpc failed." << std::endl;
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<YoloStreamer::Stub> stub_;
};

int main(int argc, char **argv)
{
    YoloClient greeter(grpc::CreateChannel("localhost:50051",
        grpc::InsecureChannelCredentials()));
    std::string message("yolo");

    std::cout << "Hello1" << std::endl;

    greeter.SayHello(message);

    return 0;
}
