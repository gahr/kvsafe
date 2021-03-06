///
// Copyright (C) 2014-2015 Pietro Cerutti <gahr@gahr.ch>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#pragma once

#include <string>
#include <unistd.h>
#include <termios.h>

struct Util
{
    enum EchoMode
    {
        ON,
        OFF
    };
    static void echo(EchoMode mode)
    {
        struct termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        if (mode == ON)
            tty.c_lflag |= ECHO;
        else
            tty.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    static bool readPassword(std::string& password, const std::string& prompt)
    {
        std::cerr << prompt << ": ";
        std::cerr.flush();
        echo(OFF);
        std::cin >> password;
        echo(ON);
        std::cerr << "\n";
        if (std::cin.eof())
        {
            std::cin.clear();
            return false;
        }
        return true;
    }
};
