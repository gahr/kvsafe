#!/usr/bin/env tclsh

set cwd [file dirname [info script]]
set conf(kvsafe) "$cwd/../build_dir/kvsafe"
set conf(kvfile) "$cwd/.kvsafe.dat"

proc kvsafe {args} {
    exec $::conf(kvsafe) -f $::conf(kvfile) -P password {*}$args 
}

proc cleanup {} {
    file delete -force -- $::conf(kvfile)
}

proc make_animals {} {
    set animals {
        duck
        horse
        cat
        cow
        beagle
        cougar
        donkey
    }
    foreach a $animals {
        kvsafe $a name $a
    }
}

set table_height 80
set table_width 60
set table_length 120
proc make_table {} {
    kvsafe table height $::table_height
    kvsafe table width $::table_width
    kvsafe table length $::table_length
}

set chair_height 50
set chair_width 51
set chair_length 52
proc make_chair {} {
    kvsafe chair height $::chair_height
    kvsafe chair width $::chair_width
    kvsafe chair length $::chair_length
}
