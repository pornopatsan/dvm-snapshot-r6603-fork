_in_list() {
    local f what
    what="$1"
    shift
    for f in "$@"; do
        if [ "$f" = "$what" ]; then
            return 0
        fi
    done
    return 1
}

_dvm_with_com() {
    local cur com

    COMPREPLY=()
    com="$1"
    cur="${COMP_WORDS[COMP_CWORD]}"

    local basecmd SRC_LANG
    basecmd=help
    SRC_LANG=
    if _in_list "$com" convert cdv cxxdv fdv; then
        basecmd=convert
    elif _in_list "$com" compile c cxx f; then
        basecmd=compile
    elif _in_list "$com" comp cc cxxc fc; then
        basecmd=comp
    elif _in_list "$com" clink cxxlink flink link; then
        basecmd=link
    elif _in_list "$com" run cmph; then
        basecmd=run
    fi
    if _in_list "$com" fdv f fc flink; then
        SRC_LANG=f
    elif _in_list "$com" cdv c cc clink; then
        SRC_LANG=c
    elif _in_list "$com" cxxdv cxx cxxc cxxlink; then
        SRC_LANG=cxx
    fi
    local IS_OPT
    if [[ "$cur" == -* ]]; then
        IS_OPT=1
    else
        IS_OPT=0
    fi
    local CONV_OPTS_CMN CONV_OPTS_F CONV_OPTS_C CONV_OPTS COMP_OPTS LINK_OPTS
    CONV_OPTS_CMN="-o -s -p -noH -mmic -w -I -d1 -d2 -d3 -d4 -e1 -e2 -e3 -e4 -dbif1 -dbif2 -autoTfm -Ohost -noCuda -Opl -gpuO1 -collapse -cacheIdx -oneThread -rtc -dvm-entry"
    CONV_OPTS_F="-bind0 -bind1 -bufio -f90 -FI -ffo -r8 -i8 -C_Cuda -F_Cuda"
    CONV_OPTS_C="-D -U -x -no-omp -enable-indirect -use-blank -omp-reduction -v -dvm-stdio -no-void-stdio -emit-blank-handlers -less-dvmlines -save-pragmas"
    if [ "$SRC_LANG" = "f" ]; then
        CONV_OPTS="$CONV_OPTS_CMN $CONV_OPTS_F"
    elif [ "$SRC_LANG" = "c" -o "$SRC_LANG" = "cxx" ]; then
        CONV_OPTS="$CONV_OPTS_CMN $CONV_OPTS_C"
    else
        CONV_OPTS="$CONV_OPTS_CMN $CONV_OPTS_F $CONV_OPTS_C"
    fi
    COMP_OPTS="-o -c -Minfo -fPIC -fpic -fPIE -fpie -mmic -rtc -I -D -U"
    LINK_OPTS="-o -shared-dvm -shared -mmic -l -L -rdynamic"

    if [ $IS_OPT -ne 0 ]; then
        if [ $basecmd = convert ]; then
            COMPREPLY=( $(compgen -W "$CONV_OPTS" -- "$cur") )
        elif [ $basecmd = compile ]; then
            COMPREPLY=( $(compgen -W "$CONV_OPTS $COMP_OPTS $LINK_OPTS" -- "$cur") )
        elif [ $basecmd = comp ]; then
            COMPREPLY=( $(compgen -W "$COMP_OPTS $LINK_OPTS" -- "$cur") )
        elif [ $basecmd = link ]; then
            COMPREPLY=( $(compgen -W "$LINK_OPTS" -- "$cur") )
        elif [ $basecmd = run ]; then
            COMPREPLY=( $(compgen -W "-perf" -- "$cur") )
        fi
    fi
}

_dvm() {
    local cur com commands

    com="${COMP_WORDS[1]}"
    cur="${COMP_WORDS[COMP_CWORD]}"

    commands="help convert cdv cxxdv fdv compile c cxx f comp cc cxxc fc clink cxxlink flink link run cmph csdeb cpdeb fsdeb fpdeb err red trc ptrc dif size runpred ctest ftest ver pred pa doc"

    if [[ "$COMP_CWORD" == 1 ]]; then
        COMPREPLY=( $(compgen -W "$commands" -- "$cur") )
    elif _in_list "$com" $commands; then
        _dvm_with_com "$com"
    fi
    return 0
} && complete -o default -F _dvm dvm dvm-mpi dvm-omp dvm-cuda

_dvm_c() {
    _dvm_with_com c
    return 0
} && complete -o default -F _dvm_c dvm-c

_dvm_f() {
    _dvm_with_com f
    return 0
} && complete -o default -F _dvm_f dvm-f

_dvm_cxx() {
    _dvm_with_com cxx
    return 0
} && complete -o default -F _dvm_cxx dvm-cxx
