#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

class Request
{
public:
    Request()
    {
        _x = _y = _oper = 0;
    }

    //序列化
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["x"]=_x;
        root["y"]=_y;
        root["oper"]=_oper;

        Json::StyledWriter writer;
        *out=writer.write(root);
        if(out->empty())
            return false;
        
        return true;
    }
    //反序列化
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool ret = reader.parse(in,root);
        if(!ret)
            return false;
        
        _x=root["x"].asInt();
        _x=root["y"].asInt();
        _x=root["oper"].asInt();

        return true;
    }
    int X()
    {
        return _x;
    }
    int Y()
    {
        return _y;
    }
    char Oper()
    {
        return _oper;
    }
    ~Request()
    {
    }
private:
    int _x;
    int _y;
    char _oper;
};



class Response
{
public:
    Response():_result(0),_code(0)
    {

    }
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;

        Json::StyledWriter writer;
        *out = writer.write(root);
        if(out->empty())
            return false;
        return true;
    }

    // 反序列化对象
    bool Deserialize(std::string &in)
    {
        Json::Reader reader;
        Json::Value root;
        bool ret = reader.parse(in, root);
        if(!ret)
            return false;

        _result = root["result"].asInt();
        _code = root["code"].asInt();

        return true;
    }
    void SetResult(int r)
    {
        _result = r;
    }
    void SetCode(int c)
    {
        _code = c;
    }
    void Print()
    {
        std::cout << _result << "[" << _code << "]" << std::endl;
    }
    ~Response()
    {
    }
private:
    int _result;
    int _code;
};

static const std::string sep = "\r\n";

class Protocol
{
public:
    static std::string Package(const std::string &jsonstr)
    {
        if(jsonstr.empty())
            return std::string();
        std::string json_length=std::to_string(jsonstr.size());
        return json_length + sep  +jsonstr +sep;
    }

    static bool DigitSafeCheck(const std::string str)
    {
        for(int i=0;i<str.size();i++)
        {
            if(!(str[i]>='0' && str[i]<='9'))
                return false;
        }
        return true;
    }

    static int Unpack(std::string &origin_str,std::string *package)
    {
        if(!package)
        {
            return 0;
        }
        auto pos=origin_str.find(sep);
        if(pos == std::string::npos)
        {
            return 0;
        }

        //代码能执行到这里就说明收到了至少有长度的报文
        std::string len_str = origin_str.substr(0,pos);
        if(!DigitSafeCheck(len_str))
        {
            return -1;
        }

        int digit_len = std::stoi(len_str);
        int target_len = len_str.size() + digit_len + 2*sep.size();

        if(origin_str.size() < target_len)
        {
            return 0;
        }

        //接下来就对origin_str进行操作
        *package = origin_str.substr(pos + sep.size(),digit_len);
        origin_str.erase(0,target_len);
        return package->size();
    }
};