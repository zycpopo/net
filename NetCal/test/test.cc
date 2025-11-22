#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

int main()
{
    // 序列化
    Json::Value root;
    root["name"] = "popo";
    root["age"] = 19;
    root["sex"] = "男";

    Json::StyledWriter writer;
    std::string s = writer.write(root);
    std::cout << s;

    //   Json::StreamWriterBuilder wbuilder;
    //   std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());

    //   std::stringstream ss;
    //   writer->write(root,&ss);

    //   std::cout << ss.str();

    // std::string ss = root.toStyledString();
    // std::cout << ss << std::endl;

    //反序列化
    Json::Value droot;
    Json::Reader reader;
    reader.parse(s,droot);

    std::string name = droot["name"].asString();
    int age = droot["age"].asInt();
    std::string sex = droot["sex"].asString();

    std::cout << name << std::endl;
    std::cout << age << std::endl;
    std::cout << sex << std::endl;


    return 0;
}