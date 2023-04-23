//
// Created by Aleksej on 23.04.2023.
//

#ifndef TRANSLATOR_HANDLERALREADYREGISTERED_HPP
#define TRANSLATOR_HANDLERALREADYREGISTERED_HPP

#include <exception>
#include <string>
#include <format>

class HandlerAlreadyRegistered : public std::exception
{
public:
    HandlerAlreadyRegistered(const std::string& handlerName, const std::string& requestName)
            : std::exception(
            std::format("Handler '{0}' already registred for '{1}' request", handlerName, requestName).c_str())
    {}
};


#endif //TRANSLATOR_HANDLERALREADYREGISTERED_HPP
