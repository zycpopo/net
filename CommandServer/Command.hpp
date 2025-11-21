#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

class Command
{
private:
    bool isSafe(const std::string &cmd)
    {
      for(auto &c:  _command_white_list)
      {
        if(cmd == c)
        {
          return true;
        }
      }

      return false;
    }
public:
    Command()
    {
        _command_white_list.push_back("ls -a -l");
        _command_white_list.push_back("ll");
        _command_white_list.push_back("cat test.txt");
        _command_white_list.push_back("touch touch.txt");
        _command_white_list.push_back("tree");
        _command_white_list.push_back("whoami");
        _command_white_list.push_back("who");
        _command_white_list.push_back("pwd");
    }
    std::string Exec(const std::string &cmd)
    {
        if(!isSafe(cmd))
        {
          return "这个命令不行";
        }
        std::string result;
        FILE *fp = popen(cmd.c_str(),"r");
        if(fp == NULL)
        {
          result = cmd + "exec error"; 
        }
        else 
        {
          char buffer[1024];
          while(fgets(buffer,sizeof(buffer),fp) != nullptr)
          {
              result +=buffer;
          }

          pclose(fp);
        }

        return result;
    }
    ~Command()
    {}
private:
    std::vector<std::string> _command_white_list;
};