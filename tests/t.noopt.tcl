package require tcltest
source [file join [::tcltest::testsDirectory] conf.tcl]

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
    -result "table.height: $::table_height"

tcltest::test noopt-1.5 {value for another key} \
    -setup { cleanup; make_table } \
    -body { kvsafe table width } \
    -cleanup { cleanup } \
    -result "table.width: $::table_width"

tcltest::test noopt-2.1 {all key for a matching entity} \
    -setup { cleanup; make_table } \
    -body { kvsafe abl } \
    -cleanup { cleanup } \
    -result "table.height\ntable.length\ntable.width"

tcltest::test noopt-2.2 {value for a matching key for a matching entity} \
    -setup { cleanup; make_table } \
    -body { kvsafe abl ei } \
    -cleanup { cleanup } \
    -result "table.height: $::table_height"

tcltest::test noopt-2.2 {multiple values for matching} \
    -setup { cleanup; make_table; make_chair } \
    -body { kvsafe "" len } \
    -cleanup { cleanup } \
    -result "chair.length: $::chair_length\ntable.length: $::table_length"

::tcltest::cleanupTests
