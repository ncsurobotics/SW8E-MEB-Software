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

# Run MSPDEBUG with all arguments passed into this script
"$MSPDEBUG" "$@"
