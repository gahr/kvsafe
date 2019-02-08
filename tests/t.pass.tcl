package require tcltest
source [file join [::tcltest::testsDirectory] conf.tcl]

tcltest::test pass-1.1 {change password} \
    -setup { cleanup } \
    -body {
        kvsafe -P oldpass entity key val
        set fd [open "|$::conf(kvsafe) -f $::conf(kvfile) -p" "w"]
        puts $fd oldpass
        puts $fd newpass
        puts $fd newpass
        catch { close $fd } msg
        if {$msg ne "Password: \nNew password: \nNew password (again): "} {
            set msg
        } else {
            exec $::conf(kvsafe) -f $::conf(kvfile) -P newpass entity key
        }
    } \
    -cleanup { cleanup } \
    -result {entity.key: val}

tcltest::test pass-1.2 {change password typo} \
    -setup { cleanup } \
    -body {
        kvsafe -P oldpass entity key val
        set fd [open "|$::conf(kvsafe) -f $::conf(kvfile) -p" "w"]
        puts $fd oldpass
        puts $fd newpass
        puts $fd newpass2
        catch { close $fd } msg
        set msg
    } \
    -cleanup { cleanup } \
    -result "Password: \nNew password: \nNew password (again): \nPasswords do not match.\nNew password: "

::tcltest::cleanupTests
