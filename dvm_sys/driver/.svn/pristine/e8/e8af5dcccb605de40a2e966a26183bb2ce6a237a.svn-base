#!/bin/sh

remove_exts() {
    if [ $# -gt 1 ]; then
        local FN
        FN="$1"
        shift
        local PAT
        PAT="$1"
        shift
        local ext
        for ext; do PAT="$PAT|$ext"; done
        PAT="$( printf '%s' "$PAT" | $sed 's/\./\\\./g' )"
        printf '%s' "$FN" | $sed -r "s/[.]($PAT)\$//gI"
    else
        printf '%s' "$1" | $sed 's/\.[^\./]*$//g'
    fi
}

has_ext() {
    local FN
    FN="$1"
    shift
    if [ $# -gt 0 ]; then
        if [ "$( remove_exts "$FN" "$@" )" != "$FN" ]; then
            return 0
        else
            return 1
        fi
    else
        if [ "$( remove_exts "$FN" )" = "$FN" ]; then
            return 0
        else
            return 1
        fi
    fi
}

add_ext() {
    local FN
    FN="$1"
    shift
    local DEF_EXT
    DEF_EXT="$1"
    shift
    if has_ext "$FN" "$@"; then
        printf '%s' "$FN"
    else
        printf '%s' "$FN.$DEF_EXT"
    fi
}

do_cmd() {
    local OUT_FN
    OUT_FN=
    if [ "$1" = "-out-to" ]; then
        OUT_FN="$2"
        shift
        shift
    fi
    if [ "$dvmshow" = "1" ]; then
        printf '%s ' "$@"
        echo
    fi
    if [ -z "$OUT_FN" ]; then
        "$@"
    else
        "$@" >"$OUT_FN" 2>& 1
    fi
}

is_number() {
    case "$1" in
        ''|*[!0-9]*) return 1 ;;
        *) return 0 ;;
    esac
}

get_first_non_number() {
    while is_number "$1"; do
        shift
    done
    printf '%s' "$1"
}

guess_language() {
    local FN
    FN="$1"
    if has_ext "$FN" $C_EXTS ; then
        echo "c"
    elif has_ext "$FN" $CXX_EXTS ; then
        echo "cxx"
    elif has_ext "$FN" $FDV_EXTS ; then
        echo "f"
    else
        if [ -e "$FN.fdv" -a ! -e "$FN.cdv" ]; then
            echo "f"
        elif [ -e "$FN.cdv" -a ! -e "$FN.fdv" ]; then
            echo "c"
        else
            echo
        fi
    fi
}

gen_taskname() {
    local count
    local name
    local exists
    name=$( basename "$1" )
    count=0
    exists=1
    while [ $exists -ne 0 ]; do
        count=$(( count + 1 ))
        exists=$( find . -type d -name "$name.$2.$count.[0-9]*" | wc -l )
        if [ $exists -eq 0 ] && [ -e "$name.$2.$count" ]; then
            exists=1
        fi
    done
    printf '%s' "$name.$2.$count"
}

C_EXTS="cdv c h"
CXX_EXTS="cpp cc hpp"
CDV_EXTS="$C_EXTS $CXX_EXTS"
F_EXTS="fdv hpf f ftn for"
F90_EXTS="f90 f95 f03"
FDV_EXTS="$F_EXTS $F90_EXTS"
SRC_EXTS="$CDV_EXTS $FDV_EXTS"

if [ "$WIN32" = "1" ]; then
    OBJ_EXT="obj"
    LIB_EXT="lib"
    EXE_SUFFIX=".exe"
    DEFAULT_EXENAME="a.exe"
else
    OBJ_EXT="o"
    LIB_EXT="a"
    EXE_SUFFIX=
    DEFAULT_EXENAME="a.out"
fi
