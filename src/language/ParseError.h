#pragma once

#include <string>

#include "Token.h"

struct ParseError {
    Token token;
    std::string message;
};
