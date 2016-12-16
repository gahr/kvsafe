package require tcltest
source [file join [::tcltest::testsDirectory] conf.tcl]

tcltest::test pass-1.1 {change password} \
    -setup { cleanup } \
    -body {
        exec $::conf(kvsafe) -f $::conf(kvfile) -P oldpass entity key val
        set fd [open "|$::conf(kvsafe) -f $::conf(kvfile) -p" "w"]
        puts $fd oldpass
        puts $fd newpass
        catch { close $fd } msg
        if {$msg ne "Password: \nNew password: "} {
            set msg
        } else {
            exec $::conf(kvsafe) -f $::conf(kvfile) -P newpass entity key
        }
    } \
    -cleanup { cleanup } \
    -result {entity.key: val}

::tcltest::cleanupTests
