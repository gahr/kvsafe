package require tcltest
source [file join [::tcltest::testsDirectory] conf.tcl]

set animals {
    duck
    horse
    cat
    cow
    beagle
    cougar
    donkey
}

proc make_animals {} {
    foreach a $::animals {
        kvsafe $a name $a
    }
}

tcltest::test entity-1.1 {exact match} \
    -setup { cleanup; make_animals } \
    -body { kvsafe -e cow } \
    -cleanup { cleanup } \
    -result {cow}

tcltest::test entity-1.2 {glob match} \
    -setup { cleanup; make_animals } \
    -body { kvsafe -e c } \
    -cleanup { cleanup } \
    -result "cat\ncougar\ncow\nduck"
