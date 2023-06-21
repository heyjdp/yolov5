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
#include <sstream>

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
    
    // The data needs to be parsed from the output string as follows:
    //
    // The object YoloReply reply.detection() is a string group that needs to be parsed
    //
    // CAM is the camera number, for our particular setup CAM:0 is RGB and CAM:1 is IR
    // SIZ is the frame size of the camera
    // OBJ is an object detection in the form:
    // tl_x, tl_y, bb_w, bb_h, object_class, confidence
    // - tl_x = top left x coordinate (interval 0..1)
    // - tl_y = top left y coordinate (interval 0..1)
    // - bb_w = bounding box width (interval 0..1)
    // - bb_h = bounding box height (interval 0..1)
    // - object_class = human readable name of the object detected
    // - confidence = confidence in recognition (interval 0..1)
    //
    // Example YOLO reply to parse:
    //
    // CAM:0
    // SIZ:(600, 800)
    // OBJ:0.1693750023841858,0.8333333134651184,0.2837499976158142,0.11999999731779099,keyboard,0.26
    // OBJ:0.42250001430511475,0.6816666722297668,0.02500000037252903,0.07999999821186066,book,0.30
    // OBJ:0.40312498807907104,0.6783333420753479,0.026249999180436134,0.07999999821186066,book,0.30
    // OBJ:0.5412499904632568,0.8725000023841858,0.20499999821186066,0.09833333641290665,book,0.33
    // OBJ:0.512499988079071,0.9574999809265137,0.125,0.07833333313465118,remote,0.39
    // OBJ:0.7737500071525574,0.7475000023841858,0.45249998569488525,0.4350000023841858,person,0.87
    // OBJ:0.2150000035762787,0.7008333206176758,0.4300000071525574,0.57833331823349,laptop,0.90
    // CAM:1
    // SIZ:(512, 640)
    // OBJ:0.792187511920929,0.720703125,0.4124999940395355,0.55859375,person,0.26
    //
    // *NOTE* it is common that especially the IR camera will have zero OBJ objects after it

    void YoloDetect(const std::string &message)
    {
        YoloRequest request;
        request.set_message(message);

        ClientContext context;
        std::unique_ptr<ClientReader<YoloReply>> reader(stub_->yoloDetection(&context, request));

        std::vector<std::string> delimited;
        YoloReply reply;
        while (reader->Read(&reply))
        {
            std::stringstream ss(reply.detection());
            while (ss.good())
            {
                std::string substr;
                getline(ss, substr, ';'); // split on semicolon
                delimited.push_back(substr);
            }
            for (auto it = delimited.begin(); it != delimited.end(); ++it)
            {
                std::cout << *it << std::endl; // Here is the actual sentences
            }
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

    greeter.YoloDetect(message);

    return 0;
}
