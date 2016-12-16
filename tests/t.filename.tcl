package require tcltest
source [file join [::tcltest::testsDirectory] conf.tcl]

proc mycleanup {} {
    file delete -force f1.dat f2.dat
}

tcltest::test filename-1.1 {specify a filename} \
    -setup { mycleanup } \
    -body {
        exec $::conf(kvsafe) -f f1.dat -P pass entity key value
        file rename f1.dat f2.dat
        exec $::conf(kvsafe) -f f2.dat -P pass entity key
    } \
    -cleanup { mycleanup } \
    -result {entity.key: value}
