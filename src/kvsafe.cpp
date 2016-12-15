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
#include <stdexcept>
#include <unistd.h>

#define USAGE do { usage(); return 1; } while (0)

static void usage()
{
    std::cerr <<
R"kvsafe_help(
kvsafe [options] [<entity> [<key>]]
    Emit all entities, all keys of all matching entity, or the value of all matching entities and keys.

kvsafe [options] <entity> <key> <value>
    Set the value of an entity's key.

kvsafe [options] {-e} [<entity>]
    Emit all entities matching a specified entity.

kvsafe [options] {-k} [<entity> [<key>]]
    Emit the keys of matching entity and key.

kvsafe [options] {-v} [<entity> [<key>]]
    Emit the values of matching entity and key.

kvsafe [options] {-d} <entity> [<key>]
    Delete a whole entity or one of its keys.

kvsafe [options] {-p}
    Change the password of the database.

kvsafe [options] {-h}
    Print this help message.

OPTIONS

    -f filename
        Use the specified database file instead of ~/.kvsafe.dat.

    -P password
        Specify a password on the command line.
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
        BULK,
        DEFAULT,
        DELETE,
        CHPASS,
        ENTITIES,
        KEYS,
        VALS,
        HELP
    } mode { DEFAULT };
    
    int ch;
    bool done = false;
    while ((ch = getopt(argc, argv, "bdef:hkpP:v-")) != -1 && !done)
    {
        switch (ch)
        {
            case 'P':
                store.setPassword(optarg);
                break;

            case 'b':
                mode = BULK;
                break;

            case 'd':
                mode = DELETE;
                break;

            case 'e':
                mode = ENTITIES;
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
                USAGE;
        }
    }

    argc -= optind;
    argv += optind;

    store.setFilename(filename);

    auto loadOrThrow = [&store]() {
        if (!store.load())
        {
            throw std::runtime_error("cannot open store");
        }
    };

    try
    {
        switch (mode)
        {
            case BULK:
                if (argc != 0)
                    USAGE;

                loadOrThrow();
                while (1)
                {
                    std::array<std::string, 3> triple;
                    for (size_t i = 0; i < 3; ++i)
                    {
                        std::getline(std::cin, triple[i]);
                    }
                    if (std::cin.eof())
                    {
                        break;
                    }
                    store.set(triple[0], triple[1], triple[2]);
                }
                store.save();
                break;

            case DEFAULT:
                switch (argc)
                {
                    case 0:
                        loadOrThrow();
                        store.emitEntities();
                        break;

                    case 1:
                        loadOrThrow();
                        store.emitProps(argv[0]);
                        break;

                    case 2:
                        loadOrThrow();
                        store.emitValues(argv[0], argv[1]);
                        break;

                    case 3:
                        loadOrThrow();
                        store.set(argv[0], argv[1], argv[2]);
                        store.save();
                        break;

                    default:
                        USAGE;
                }
                break;

            case ENTITIES:
                if (argc > 1)
                    USAGE;

                loadOrThrow();
                store.emitEntities(argv[0] ? argv[0] : std::string());
                break;

            case KEYS:
                if (argc > 2)
                    USAGE;

                loadOrThrow();
                store.emitProps(argv[0] ? argv[0] : std::string(),
                                (argv[0] && argv[1]) ? argv[1] : std::string());
                break;

            case VALS:
                if (argc > 2)
                    USAGE;

                loadOrThrow();
                store.emitValues(argv[0] ? argv[0] : std::string(),
                                (argv[0] && argv[1]) ? argv[1] : std::string());
                break;

            case CHPASS:
                if (argc != 0)
                    USAGE;

                loadOrThrow();
                store.changePassword();
                store.save();
                break;

            case DELETE:
                switch (argc)
                {
                    case 1:
                        loadOrThrow();
                        store.removeEntity(argv[0]);
                        store.save();
                        break;

                    case 2:
                        loadOrThrow();
                        store.removeProp(argv[0], argv[1]);
                        store.save();
                        break;

                    default:
                        USAGE;
                }
                break;

            case HELP:
                USAGE;
        }
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
