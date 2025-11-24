#pragma once

#include <iostream>
#include <string>
#include <functional>

#include "protocol.hpp"
#include "Logger.hpp"

using handler_t = std::function<Response(Request &req)>;

class Parser
{
public:
    Parser(handler_t handler):_handler(handler)
    {}
    std::string Parse(std::string &inbuffer)
    {
        LOG(LogLevel::DEBUG) << "inbuffer: \r\n" <<inbuffer;
        std::string send_str;
        while(true)
        {
            std::string jsonstr;

            int n = Protocol::Unpack(inbuffer,&jsonstr);
            if(n < 0)
                exit(0);
            else if(n == 0)
                break;
            else
            {
                LOG(LogLevel::DEBUG) << "jsonstr:: \r\n" <<jsonstr;
                Request req;
                if(!req.Deserialize(jsonstr))
                {
                    return std::string();
                }

                Response resp = _handler(req);

                std::string resp_json;
                if(!resp.Serialize(&resp_json))
                {
                    return std::string();
                }

                send_str += Protocol::Package(resp_json);
            }
            return send_str;
        }
    }
    ~Parser()
    {}
private:
    handler_t _handler;
};