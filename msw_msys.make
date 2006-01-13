# Platform specifics: msw using MSYS. Study IMPORTANT NOTES below!
#
# Copyright (C) 2005, 2006 Gregory W. Chicares.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# http://savannah.nongnu.org/projects/lmi
# email: <chicares@cox.net>
# snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

# $Id: msw_msys.make,v 1.10 2006-01-13 12:39:45 chicares Exp $

################################################################################

# MSYS !! IMPORTANT NOTES.
#
# MSYS poses certain problems that must be worked around carefully.
# These problems have been confirmed with MSYS-1.0.10 of 2004-05,
# the latest "release" as of 2006-01. Studying this comment block
# assiduously will save you time. Rereading it when something goes
# wrong will also save you time.
#
# Its sed dates from 1998 and can't handle modern usage. It must be
# replaced as follows:
#  - Get the GNU sources for sed-4.0.7 (tested) or later (untested).
#  - Build a new msw binary in MSYS with './configure && make'.
#  - Copy it to MSYS path '$(system_root)/usr/bin/sed' to use the
#    definition of $(SED) below (or supply your own definition).
#
# Do not copy this new sed binary into MSYS's own /bin/ or /usr/bin/
# directories: that would certainly cause horrible problems. Hesitate
# to place any file there: read the cautions in MSYS's /doc/msys/* .
# Don't read '/doc/msys/README.rtf' alone and skip the rest: this
# particular pitfall is documented in '/doc/msys/MSYS_WELCOME.rtf'.
# Don't replace or delete the MSYS-provided '/bin/sed': that would
# break your MSYS installation for packages that can use that ancient
# version of sed.
#
# Replacing sed exposes a nasty problem in MSYS's bash port, which
# "translates" any program argument it deems to be a path; details:
#   http://sf.net/mailarchive/message.php?msg_id=10668423
# Here's another example:
#   http://sf.net/mailarchive/message.php?msg_id=9879502
# Follow those links to learn how to recognize this problem. AFAIK,
# the problems have been worked around by changing sed commands
# throughout the lmi makefiles in benign but ridiculous ways.
#
# If you modify MSYS's '/etc/fstab', end each line with '\n', and
# don't insert any '\r' or use any editor that will do that. Check
# your work carefully with 'od -t a /etc/fstab'.
#
# Don't ever use any path with embedded spaces anywhere.
#
# Don't attempt to mix cygwin and MSYS tools. Don't permit $PATH to
# point to one when you want to use the other.
#
# Use this command
#   echo '"\e[3~": delete-char' >>~/.inputrc
# to make the Delete key work.

################################################################################

# This may be overridden on the command line, or by providing a
# definition in the environment, e.g., "export system_root='/d'".

system_root ?= /c

################################################################################

# Set normal system paths explicitly unless $(USE_STD_PATHS) is specified.
# Reason: many problems reported on mailing lists are due to users mixing
# MSYS and cygwin tools by setting $(PATH) incorrectly; but people who set
# $(USE_STD_PATHS) are assumed to know what they're doing. These paths are
# slash-terminated so that setting them to empty strings works, too.

# MSYS mounts /usr/bin/ as an alias for /bin/ , so it's irrelevant that
# it places files that belong in /usr/bin/ physically in its /bin/ . This
# has nothing to do with $(PATH).

ifeq (,$(USE_STD_PATHS))
  PATH_BIN     := /bin/
  PATH_GCC     := /mingw/bin/
  PATH_USR_BIN := /usr/bin/
endif

################################################################################

# Compiler, linker, and so on.

AR     := $(PATH_GCC)ar
CC     := $(PATH_GCC)gcc
CPP    := $(PATH_GCC)cpp
CXX    := $(PATH_GCC)g++
LD     := $(PATH_GCC)g++
RC     := $(PATH_GCC)windres

################################################################################

# Standard utilities.

# Required in /bin (if anywhere) by FHS-2.2 .

CP      := $(PATH_BIN)cp
DATE    := $(PATH_BIN)date
ECHO    := $(PATH_BIN)echo
GZIP    := $(PATH_BIN)gzip
LS      := $(PATH_BIN)ls
MKDIR   := $(PATH_BIN)mkdir
MV      := $(PATH_BIN)mv
RM      := $(PATH_BIN)rm

SED     := $(system_root)/usr/bin/sed

TAR     := $(PATH_BIN)tar

# FHS-2.2 would put these in /usr/bin .

BZIP2   := $(PATH_USR_BIN)bzip2
DIFF    := $(PATH_USR_BIN)diff
GREP    := $(PATH_USR_BIN)grep
MD5SUM  := $(PATH_USR_BIN)md5sum
PATCH   := $(PATH_USR_BIN)patch
TOUCH   := $(PATH_USR_BIN)touch
TR      := $(PATH_USR_BIN)tr
WC      := $(PATH_USR_BIN)wc
WGET    := $(PATH_USR_BIN)wget

# TODO ?? Where would libxml2 install this thing?
# XMLLINT := $(PATH_USR_BIN)xmllint
XMLLINT := $(system_root)/usr/bin/xmllint

################################################################################

# Configuration shared by all msw subplatforms.

include $(src_dir)/msw_common.make
$(src_dir)/msw_common.make:: ;

