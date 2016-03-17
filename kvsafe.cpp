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
#include "consoler/simple.h"

#include <unistd.h>

static void usage()
{
    static const char * p = "kvsafe";
    std::cout
        << p << "\n"
             << "\tEmit all entities.\n\n"
        << p << " e\n"
             << "\tEmit all keys for entity e.\n\n"
        << p << " e k\n"
             << "\tEmit the value of entity e's key k.\n\n"
        << p << " e k v\n"
             << "\tSet or change to v the value of entity's e key k.\n\n"
        << p << " -d e\n"
             << "\tRemove entity e.\n\n"
        << p << " -d e k\n"
             << "\tRemove key k of entity e.\n\n"
        << p << " -e\n"
             << "\tExport the whole database.\n\n"
        << p << " -p\n"
             << "\tChange the password.\n\n"
        << p << " -h\n"
             << "\tPrint this help message.\n\n";
}

int main(int argc, char ** argv)
{
    Store<Filer::Simple, Consoler::Xo> store(argc, argv);

    std::string filename { ".kvsafe.dat" };

    enum {
        DEFAULT,
        DELETE,
        EXPORT,
        CHPASS,
        HELP
    } mode { DEFAULT };
    
    int ch;
    while ((ch = getopt(argc, argv, "def:hp")) != -1)
    {
        switch (ch)
        {
            case 'd':
                mode = DELETE;
                break;

            case 'e':
                mode = EXPORT;
                break;

            case 'f':
                filename = optarg;
                break;

            case 'h':
                mode = HELP;
                break;

            case 'p':
                mode = CHPASS;
                break;
        }
    }

    argc -= optind;
    argv += optind;

    store.setFilename(filename);

    switch (mode)
    {
        case DEFAULT:
            switch (argc)
            {
                case 0:
                   store.load();
                   store.emitEntities();
                   break;

                case 1:
                  store.load();
                  store.emitProps(argv[0]);
                  break;

                case 2:
                  store.load();
                  store.emitValue(argv[0], argv[1]);
                  break;

                case 3:
                  store.load();
                  store.set(argv[0], argv[1], argv[2]);
                  store.save();
                  break;

                default:
                  usage();
                  break;
            }
            break;

        case CHPASS:
            switch (argc)
            {
                case 0:
                    store.load();
                    store.changePassword();
                    store.save();
                    break;

                default:
                    usage();
                    break;
            }
            break;

        case DELETE:
            switch (argc)
            {
                case 0:
                    usage();
                    break;

                case 1:
                    store.load();
                    store.removeEntity(argv[0]);
                    store.save();
                    break;

                case 2:
                    store.load();
                    store.removeProp(argv[0], argv[1]);
                    store.save();
                    break;

                default:
                    usage();
                    break;
            }
            break;

        case EXPORT:
            switch (argc)
            {
                case 0:
                    store.load();
                    for (const auto& e : store.entities())
                    {
                        for (const auto&p : e.props())
                        {
                            store.emitValue(e.name(), p.name());
                        }
                    }
                    break;

                default:
                    usage();
                    break;
            }
            break;

        case HELP:
            usage();
            break;
    }
}
