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

#include "store.h"
#include "filer/simple.h"
#include "consoler/xo.h"

#include <unistd.h>

int main(int argc, char ** argv)
{
    Store<Filer::Simple, Consoler::Xo> store(argc, argv);

    std::string filename { ".kvsafe.dat" };

    enum {
        DEFAULT,
        DELETE,
        CHPASS
    } mode { DEFAULT };
    
    int ch;
    while ((ch = getopt(argc, argv, "df:p")) != -1)
    {
        switch (ch)
        {
            case 'd':
                mode = DELETE;
                break;

            case 'f':
                filename = optarg;
                break;

            case 'p':
                mode = CHPASS;
                break;
        }
    }

    argc -= optind;
    argv += optind;

    store.setFilename(filename);
    store.load();

    switch (mode)
    {
        case DEFAULT:
            switch (argc)
            {
                case 0: store.emitEntities(); break;
                case 1: store.emitProps(argv[0]); break;
                case 2: store.emitValue(argv[0], argv[1]); break;
                case 3: store.set(argv[0], argv[1], argv[2]); break;
                default: break;
            }
            break;

        case CHPASS:
            store.changePassword();
            break;

        case DELETE:
            switch (argc)
            {
                case 0: break;
                case 1: store.removeEntity(argv[0]); break;
                case 2: store.removeProp(argv[0], argv[1]); break;
                default: break;
            }
            break;
    }

    store.save();
}
