package require tcltest
source [file join [::tcltest::testsDirectory] conf.tcl]

set t_height 80
set t_width 60
set t_length 120
proc make_table {} {
    kvsafe table height $::t_height
    kvsafe table width $::t_width
    kvsafe table length $::t_length
}

set c_height 50
set c_width 51
set c_length 52
proc make_chair {} {
    kvsafe chair height $::c_height
    kvsafe chair width $::c_width
    kvsafe chair length $::c_length
}

tcltest::test noopt-1.1 {add an entry} \
    -setup { cleanup } \
    -body {
        kvsafe entity key value
        kvsafe entity key
    } \
    -cleanup { cleanup } \
    -result {entity.key: value}

tcltest::test noopt-1.2 {change an existing entry} \
    -setup { cleanup } \
    -body {
        kvsafe entity key val1
        append msg [kvsafe entity key]
        kvsafe entity key val2
        append msg " | "
        append msg [kvsafe entity key]
        set msg
    } \
    -cleanup { cleanup } \
    -result {entity.key: val1 | entity.key: val2}

tcltest::test noopt-1.3 {all key of an entity} \
    -setup { cleanup; make_table } \
    -body { kvsafe table } \
    -cleanup { cleanup } \
    -result "table.height\ntable.length\ntable.width"

tcltest::test noopt-1.4 {value for a key} \
    -setup { cleanup; make_table } \
    -body { kvsafe table height } \
    -cleanup { cleanup } \
    -result "table.height: $::t_height"

tcltest::test noopt-1.5 {value for another key} \
    -setup { cleanup; make_table } \
    -body { kvsafe table width } \
    -cleanup { cleanup } \
    -result "table.width: $::t_width"

tcltest::test noopt-2.1 {all key for a matching entity} \
    -setup { cleanup; make_table } \
    -body { kvsafe abl } \
    -cleanup { cleanup } \
    -result "table.height\ntable.length\ntable.width"

tcltest::test noopt-2.2 {value for matching a key for a matching entity} \
    -setup { cleanup; make_table } \
    -body { kvsafe abl ei } \
    -cleanup { cleanup } \
    -result "table.height: $::t_height"

tcltest::test noopt-2.2 {multiple values for matching} \
    -setup { cleanup; make_table; make_chair } \
    -body { kvsafe "" len } \
    -cleanup { cleanup } \
    -result "chair.length: $::c_length\ntable.length: $::t_length"

::tcltest::cleanupTests
