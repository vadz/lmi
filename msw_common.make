# Platform specifics: msw, shared by all subplatforms.
#
# Copyright (C) 2005, 2006, 2007 Gregory W. Chicares.
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
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
#
# http://savannah.nongnu.org/projects/lmi
# email: <chicares@cox.net>
# snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

# $Id: msw_common.make,v 1.14 2007-01-27 00:00:51 wboutin Exp $

################################################################################

EXEEXT := .exe
SHREXT := .dll

################################################################################

# Libraries.

# There is no universal standard way to install free software on this
# platform, so copy libraries and their headers to /usr/local as FHS
# prescribes.

# Prefer to use a shared-library version of libxml2: it links faster
# and rarely needs to be updated.

platform_defines := \
  -DLIBXML_USE_DLL \
  -DSTRICT \

platform_boost_libraries := \
  -lboost_filesystem-mgw \

platform_gnome_xml_libraries := \
  -lxslt.dll \
  -lxml2.dll \

platform_mpatrol_libraries := \
  -limagehlp \

platform_wx_libraries := \
  -lwx_new \
  -lwxmsw25d \

platform_gui_ldflags := -mwindows

################################################################################

# HTML server's cgi-bin directory. Not used yet. Eventually, an
# 'install' target might copy cgi-bin binaries thither.
#
#cgi_bin_dir := $(system_root)/unspecified/cgi-bin

