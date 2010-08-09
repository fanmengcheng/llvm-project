"""
Test lldb help command.

See also CommandInterpreter::OutputFormattedHelpText().
"""

import os, time
import unittest2
import lldb
from lldbtest import *

class TestHelpCommand(TestBase):

    mydir = "help"

    def test_simplehelp(self):
        """A simple test of 'help' command and its output."""
        res = lldb.SBCommandReturnObject()
        self.ci.HandleCommand("help", res)
        #time.sleep(0.1)
        self.assertTrue(res.Succeeded())
        self.assertTrue(res.GetOutput().startswith(
            'The following is a list of built-in, permanent debugger commands'),
                        CMD_MSG('help'))

    def test_help_should_not_hang_emacsshell(self):
        """Command 'set term-width 0' should not hang the help command."""
        res = lldb.SBCommandReturnObject()
        self.ci.HandleCommand("set term-width 0", res)
        #time.sleep(0.1)
        self.assertTrue(res.Succeeded(), CMD_MSG('set term-width 0'))
        self.ci.HandleCommand("help", res)
        #time.sleep(0.1)
        self.assertTrue(res.Succeeded())
        self.assertTrue(res.GetOutput().startswith(
            'The following is a list of built-in, permanent debugger commands'),
                        CMD_MSG('help'))


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
