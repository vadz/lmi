#!/bin/sh

# Create a chroot for cross-building "Let me illustrate..." on centos-7.
#
# Copyright (C) 2016, 2017, 2018, 2019, 2020 Gregory W. Chicares.
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
# email: <gchicares@sbcglobal.net>
# snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

. ./lmi_setup_inc.sh
. /tmp/schroot_env

set -evx

assert_su
assert_not_chrooted

./lmi_setup_05c.sh

./lmi_setup_07r.sh

# BEGIN ./lmi_setup_10.sh
yum --assumeyes install debootstrap schroot
# END   ./lmi_setup_10.sh

# BEGIN ./lmi_setup_11.sh
# Cache apt archives for the chroot's debian release, to save a great
# deal of bandwidth if multiple chroots are created with the same
# release. Do this:
#   - before invoking 'debootstrap' (or 'apt-get' in the chroot),
#     so that all packages are cached; and
#   - while not chrooted, so that the host filesystem is accessible.
# The alternative of rbind-mounting parent directory var/cache/apt
# might be investigated.
CACHEDIR=/var/cache/"${CODENAME}"
mkdir -p "${CACHEDIR}"

# Bootstrap a minimal debian system. Options:
#   --include=zsh, because of "shell=/bin/zsh" below
#   --variant=minbase, as explained here:
#     https://lists.nongnu.org/archive/html/lmi/2020-05/msg00026.html
mkdir -p /srv/chroot/"${CHRTNAME}"
debootstrap --arch=amd64 --cache-dir="${CACHEDIR}" \
 --variant=minbase --include=zsh \
 "${CODENAME}" /srv/chroot/"${CHRTNAME}" >"${CHRTNAME}"-debootstrap-log 2>&1

# This command should produce no output:
grep --invert-match '^I:' "${CHRTNAME}"-debootstrap-log

echo Installed debian "${CODENAME}" chroot.
# END   ./lmi_setup_11.sh
