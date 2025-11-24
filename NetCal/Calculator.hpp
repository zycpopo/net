#pragma once

#include "protocol.hpp"

#include <iostream>
#include <string>

class Calculator
{
public:
    Calculator() {}
    Response Exec(Request &req)
    {
        Response resp;
        switch (req.Oper())
        {
        case '+':
            resp.SetResult(req.X() + req.Y());
            break;
        case '-':
            resp.SetResult(req.X() - req.Y());
            break;
        case '*':
            resp.SetResult(req.X() * req.Y());
            break;
        case '/':
        {
            if (req.Y() == 0)
            {
                resp.SetCode(1); // 1:div 0
            }
            else
            {
                resp.SetResult(req.X() / req.Y());
            }
        }
        break;
        case '%':
        {
            if (req.Y() == 0)
            {
                resp.SetCode(2); // 1:mod 0
            }
            else
            {
                resp.SetResult(req.X() % req.Y());
            }
        }
        break;
        // case '^':
        //     /* code */
        //     break;
        // case '|':
        //     /* code */
        //     break;
        // case '&':
        //     /* code */
        //     break;
        default:
            resp.SetCode(3); // 3: 非法操作
            break;
        }
        return resp;
    }
    ~Calculator() {}
};