#!/bin/sh
# =============================================================================
# portable_find_ls: fls.sh
#
# Cross-platform recursive listing with OS-appropriate security / metadata
# flags, modelled on the Linux one-liner:
#
#   find <path> -exec ls -lahFZd {} +
#
# Detected OS               Extra flag    What is shown
# ------------------------- ------------- ------------------------------------
# Linux (GNU coreutils)      -Z            SELinux / smack security context
# macOS (Darwin)             -O            BSD file flags (hidden, uchg, …)
# FreeBSD                    getfmac(8)    Mandatory Access Control labels
# NetBSD / OpenBSD           -o            BSD file flags
# Solaris / Illumos          -@            Extended-attribute marker
# Generic POSIX              (none)        Fallback — no extra metadata
#
# Usage:
#   fls.sh [<path> ...]
#
#   If no paths are given the current directory is used.
#
# Why no  find -L ?
#   -L makes find follow and recurse INTO symlinked directories.  When it
#   hits a circular symlink it detects the loop and silently drops that
#   entry, so the symlink never appears in the listing.  Without -L, find
#   lists every symlink as a plain entry and  ls -d  then renders it as
#   lrwxrwxrwx, which is the desired output format.
#
# Notes:
#   • -exec {} +  requires POSIX.1-2008 find; present on all target OSes.
#   • FreeBSD:  a second pass with getfmac(8) is appended after the ls
#     listing, as there is no equivalent inline flag for MAC labels.
#   • On Solaris / Illumos  ls -@  marks files that carry extended attrs
#     but does not print the attr values; use runat(1) / getfattr(1) for
#     the full values.
# =============================================================================

set -u
# Note: -e (errexit) is deliberately omitted.  find(1) exits non-zero when
# it encounters circular symlinks or permission errors, which is normal and
# should not abort a multi-path run.

PROG="$(basename -- "$0")"

# -----------------------------------------------------------------------------
# usage / help
# -----------------------------------------------------------------------------
help() {
    cat <<EOF
Usage: $PROG [-h|--help] [<path> ...]

  Lists every filesystem entry under each <path> with security metadata
  appropriate to the current OS.  Defaults to the current directory.

  Detected OS            ls flags      Extra metadata
  ----------------       ----------    ----------------------------------
  Linux (GNU)            -lahFZd       SELinux / smack context  (-Z)
  macOS (Darwin)         -lahFdO       BSD file flags           (-O)
  FreeBSD                -lahFd        MAC labels via getfmac(8)
  NetBSD / OpenBSD       -lahFdo       BSD file flags           (-o)
  Solaris / Illumos      -lahFd@       Extended-attribute mark  (-@)
  Generic POSIX          -laFd         (none — fallback)

  This OS: $OS_LABEL
  Command:  find <path> -exec ls $LS_FLAGS {} +
EOF
    exit 0
}



# -----------------------------------------------------------------------------
# OS detection
# -----------------------------------------------------------------------------
OS="$(uname -s 2>/dev/null)" || OS="Unknown"

case "$OS" in

    Linux)
        # GNU ls -Z prints SELinux / Smack security labels in a column
        # between the permissions block and the owner field.
        LS_FLAGS="-lahFZd"
        OS_LABEL="Linux / GNU coreutils  (SELinux context via -Z)"
        USE_GETFMAC=0
        ;;

    Darwin)
        # macOS BSD ls -O prints extra file flags after the link count
        # (e.g. hidden, uchg, schg, uappnd, …).  There is no -Z on macOS.
        LS_FLAGS="-lahFdO"
        OS_LABEL="macOS / Darwin BSD ls  (file flags via -O)"
        USE_GETFMAC=0
        ;;

    FreeBSD)
        # FreeBSD ls has no flag equivalent to Linux -Z or macOS -O for MAC
        # labels.  Labels are queried separately with getfmac(8) and printed
        # as a supplemental block after the main listing.
        LS_FLAGS="-lahFd"
        OS_LABEL="FreeBSD  (base ls; Mandatory Access Control via getfmac)"
        USE_GETFMAC=1
        ;;

    NetBSD|OpenBSD)
        # Both NetBSD and OpenBSD ls support -o to append BSD file flags
        # (e.g. simmutable, uappnd, nodump) after the size column.
        LS_FLAGS="-lahFdo"
        OS_LABEL="${OS} / BSD ls  (file flags via -o)"
        USE_GETFMAC=0
        ;;

    SunOS)
        # Solaris / Illumos ls -@ appends an '@' to the permissions string
        # for any file that carries extended attributes.
        LS_FLAGS="-lahFd@"
        OS_LABEL="Solaris / Illumos  (extended-attribute marker via -@)"
        USE_GETFMAC=0
        ;;

    *)
        # Strict POSIX fallback: -l -a -F -d are universally portable.
        # No extended metadata flags are attempted.
        LS_FLAGS="-laFd"
        OS_LABEL="${OS}  (generic POSIX fallback — no extended metadata)"
        USE_GETFMAC=0
        ;;

esac

case "${1:-}" in -h|--help) help ;; esac

# -----------------------------------------------------------------------------
# helpers
# -----------------------------------------------------------------------------
warn()   { printf '%s: %s\n' "$PROG" "$*" >&2; }

# path_exists <p>  – true even for broken symlinks (stat fails on those)
path_exists() { test -e "$1" || test -L "$1"; }

# -----------------------------------------------------------------------------
# default to current directory if no arguments
# -----------------------------------------------------------------------------
if [ "$#" -eq 0 ]; then
    set -- "."
fi

# -----------------------------------------------------------------------------
# main listing
# -----------------------------------------------------------------------------
for _p in "$@"; do
    if ! path_exists "$_p"; then
        warn "path not found: $_p"
        continue
    fi
    if [ "$USE_GETFMAC" -eq 1 ]; then
        # FreeBSD: no ls flag for MAC labels, so run a sh -c subshell per
        # batch that calls both ls and getfmac and emits tab-separated fields.
        # A second-pass awk then measures the widest owner, group, and MAC
        # label in the listing and reprints every line with consistent padding,
        # matching the dynamic column alignment that native ls produces.
        find "$_p" -exec sh -c '
            for f do
                set -- $(ls -lahFd -- "$f" 2>/dev/null)
                mac=$(getfmac -- "$f" 2>/dev/null \
                      | sed "s/^[^:]*:[[:space:]]*//" )
                [ -z "$mac" ] && mac="-"
                printf "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
                    "$1" "$2" "$3" "$4" "$mac" "$5" "$6" "$7" "$8" "$f"
            done
        ' _ {} + | awk -F'\t' '
            { row[NR] = $0
              split($0, f, "\t")
              if (length(f[3]) > ow) ow = length(f[3])
              if (length(f[4]) > gw) gw = length(f[4])
              if (length(f[5]) > mw) mw = length(f[5])
            }
            END {
              for (i = 1; i <= NR; i++) {
                split(row[i], f, "\t")
                printf "%s %2s %-*s %-*s %-*s %5s %s %2s %5s %s\n",
                    f[1], f[2], ow, f[3], gw, f[4], mw, f[5],
                    f[6], f[7], f[8], f[9], f[10]
              }
            }
        ' || true
    else
        # All other OSes: ls carries the extra metadata flag natively.
        # Word-splitting on $LS_FLAGS is intentional.
        # shellcheck disable=SC2086
        find "$_p" -exec ls $LS_FLAGS -- {} + || true
    fi
done
