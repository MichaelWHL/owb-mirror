# Copyright (c) 2010 Google Inc. All rights reserved.
# Copyright (c) 2009 Apple Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# A tool for automating dealing with bugzilla, posting patches, committing patches, etc.

import os

from webkitpy.bugzilla import Bugzilla
from webkitpy.buildbot import BuildBot
from webkitpy.commands.download import *
from webkitpy.commands.early_warning_system import *
from webkitpy.commands.openbugs import OpenBugs
from webkitpy.commands.queries import *
from webkitpy.commands.queues import *
from webkitpy.commands.sheriffbot import *
from webkitpy.commands.upload import *
from webkitpy.executive import Executive
from webkitpy.irc.ircproxy import IRCProxy
from webkitpy.webkit_logging import log
from webkitpy.webkitcheckout import WebKitCheckout
from webkitpy.multicommandtool import MultiCommandTool
from webkitpy.scm import detect_scm_system
from webkitpy.user import User


class WebKitPatch(MultiCommandTool):
    global_options = [
        make_option("--dry-run", action="store_true", dest="dry_run", default=False, help="do not touch remote servers"),
        make_option("--status-host", action="store", dest="status_host", type="string", nargs=1, help="Hostname (e.g. localhost or commit.webkit.org) where status updates should be posted."),
        make_option("--irc-password", action="store", dest="irc_password", type="string", nargs=1, help="Password to use when communicating via IRC."),
    ]

    def __init__(self, path):
        MultiCommandTool.__init__(self)

        self._path = path
        self.bugs = Bugzilla()
        self.buildbot = BuildBot()
        self.executive = Executive()
        self._irc = None
        self.user = User()
        self._scm = None
        self._checkout = None
        self.status_server = StatusServer()

    def scm(self):
        # Lazily initialize SCM to not error-out before command line parsing (or when running non-scm commands).
        original_cwd = os.path.abspath(".")
        if not self._scm:
            self._scm = detect_scm_system(original_cwd)

        if not self._scm:
            script_directory = os.path.abspath(sys.path[0])
            self._scm = detect_scm_system(script_directory)
            if self._scm:
                log("The current directory (%s) is not a WebKit checkout, using %s" % (original_cwd, self._scm.checkout_root))
            else:
                error("FATAL: Failed to determine the SCM system for either %s or %s" % (original_cwd, script_directory))

        return self._scm

    def checkout(self):
        if not self._checkout:
            self._checkout = WebKitCheckout(self.scm())
        return self._checkout

    # FIXME: Add a parameter for nickname?
    def ensure_irc_connected(self):
        if not self._irc:
            self._irc = IRCProxy(password=self._irc_password)

    def irc(self):
        # We don't automatically construct IRCProxy here because constructing
        # IRCProxy actually connects to IRC.  We want clients to explicitly
        # connect to IRC.
        return self._irc

    def path(self):
        return self._path

    def command_completed(self):
        if self._irc:
            self._irc.disconnect()

    def should_show_in_main_help(self, command):
        if not command.show_in_main_help:
            return False
        if command.requires_local_commits:
            return self.scm().supports_local_commits()
        return True

    # FIXME: This may be unnecessary since we pass global options to all commands during execute() as well.
    def handle_global_options(self, options):
        if options.dry_run:
            self.scm().dryrun = True
            self.bugs.dryrun = True
        if options.status_host:
            self.status_server.set_host(options.status_host)
        if options.irc_password:
            self._irc_password = options.irc_password

    def should_execute_command(self, command):
        if command.requires_local_commits and not self.scm().supports_local_commits():
            failure_reason = "%s requires local commits using %s in %s." % (command.name, self.scm().display_name(), self.scm().checkout_root)
            return (False, failure_reason)
        return (True, None)
