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
