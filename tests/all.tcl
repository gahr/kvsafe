#!/usr/bin/env tclsh

package require tcltest
::tcltest::configure {*}$argv -testdir [file dir [info script]] -file "t.*.tcl"
::tcltest::runAllTests
