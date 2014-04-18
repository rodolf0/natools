#!/usr/bin/env bash

wdir=$(cd "$(dirname "$0")"; pwd)
srcdir="${1:-$wdir}"

[ -d "$srcdir" ] || exit 1

remove_eol_spaces=no
function format_source {
  if ! type -path clang-format &> /dev/null; then
    echo "clang-format is not installed" >&2
    exit 1;
  fi
  if ! [ -d "$srcdir" ]; then
    echo "source code directory not found at [$srcdir]" >&2
    exit 1;
  fi
  find "$srcdir" -iname \*.c \
              -o -iname \*.h |
    xargs -n 1 -I@ clang-format -style=LLVM -i @

  if [ "$remove_eol_spaces" = yes ]; then
    find "$srcdir" -iname \*.c \
                -o -iname \*.h |
      xargs -n 1 -I@ sed -i 's!\s*$!!' @
  fi
}

function check_malloc {
  find "$srcdir" -iname \*.c \
              -o -iname \*.h |
    while read f; do
      egrep -q -w 'malloc|realloc' "$f" &&
        echo "$f not using zmalloc/xrealloc" >&2
    done
}

#format_source
check_malloc
