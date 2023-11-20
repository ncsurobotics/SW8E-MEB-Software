#!/usr/bin/env sh

# Get path to mspdebug the same way toolchain file gets path for compiler
if [ -z "$MSPDEBUG_ROOT" ]; then
    uanme_s="$(uname -s)"
    case "${uanme_s}" in
        Darwin*) MSPDEBUG="/Applications/ti/mspdebug/mspdebug" ;;
        *) MSPDEBUG="/opt/ti/mspdebug/mspdebug" ;;
    esac
else
    MSPDEBUG="$MSPDEBUG_ROOT/mspdebug"
fi

# First arg to this script is the elf file name
# Change extension from elf to hex
# Then program the hex and run gdb
# VSCode can't subtitute elf for hex, so can't do this directly in IDE
FILE=$(echo "$1" | sed 's/.elf/.hex/g')
"$MSPDEBUG" --allow-fw-update tilib --force-reset "prog $FILE" "gdb"
