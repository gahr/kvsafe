///
// Copyright (C) 2014-2016 Pietro Cerutti <gahr@gahr.ch>
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

#include <cstdlib>
#include <unistd.h>

static void usage()
{
    std::cout <<
R"kvsafe_help(
kvsafe [options] [<entity> [<key>]]
    Emit all entities, all keys of an entity, or the value of an entity's key.

kvsafe [options] <entity> <key> <value>
    Set the value of an entity's key.

kvsafe [options] {-k} [<entity>]
    Emit the keys of all entities, or of a specified entity.

kvsafe [options] {-v} [<entity>]
    Emit the values of all keys of all entities, or of a specified entity.

kvsafe [options] {-d} <entity> [<key>]
    Delete a whole entity or one of its keys.

kvsafe [options] {-p}
    Change the password of the database.

kvsafe [options] {-h}
    Print this help message.

OPTIONS

    -f filename
        Use the specified database file instead of ~/.kvsafe.dat.

    --
        Mark the end of the options. Allows entity names starting with a dash.
)kvsafe_help";
}

int main(int argc, char ** argv)
{
    Store<Filer::Simple, Consoler::Xo> store(argc, argv);

    std::string filename { std::getenv("HOME") };
    filename.append("/.kvsafe.dat");

    enum {
        DEFAULT,
        DELETE,
        CHPASS,
        KEYS,
        VALS,
        HELP
    } mode { DEFAULT };
    
    int ch;
    bool done = false;
    while ((ch = getopt(argc, argv, "df:hkpv-")) != -1 && !done)
    {
        switch (ch)
        {
            case 'd':
                mode = DELETE;
                break;

            case 'f':
                filename = optarg;
                break;

            case 'h':
                mode = HELP;
                break;

            case 'k':
                mode = KEYS;
                break;

            case 'p':
                mode = CHPASS;
                break;

            case 'v':
                mode = VALS;
                break;

            case '-':
                done = true;
                break;

            default:
                usage();
                return 1;
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
                  store.emitValues(argv[0], argv[1]);
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

        case KEYS:
            store.load();
            store.emitProps(argv[0] ? argv[0] : std::string());
            break;

        case VALS:
            store.load();
            store.emitValues(argv[0] ? argv[0] : std::string());
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

        case HELP:
            usage();
            break;
    }
}
