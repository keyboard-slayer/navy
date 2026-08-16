set -u

RED=$'\033[31m'
GREEN=$'\033[32m'
YELLOW=$'\033[33m'
BOLD=$'\033[1m'
DIM=$'\033[2m'
RESET=$'\033[0m'

NAME_WIDTH=18
fail_count=0
total_count=0

failed() {
    local name=$1 required=${2:-}
    if [ -n "$required" ]; then
        printf "  ${RED}✗${RESET} %-${NAME_WIDTH}s ${DIM}not found or too old (>= %s required)${RESET}\n" "$name" "$required"
    else
        printf "  ${RED}✗${RESET} %-${NAME_WIDTH}s ${DIM}not found${RESET}\n" "$name"
    fi
    fail_count=$((fail_count + 1))
}

success() {
    local name=$1
    printf "  ${GREEN}✓${RESET} %-${NAME_WIDTH}s\n" "$name"
}

commandIsAvailable() {
    local cmd=$1 min_major=${2:-}
    total_count=$((total_count + 1))

    if ! command -v "$cmd" >/dev/null 2>&1; then
        failed "$cmd"
        return
    fi

    if [ -z "$min_major" ]; then
        success "$cmd"
        return
    fi

    local out major
    out=$("$cmd" --version 2>&1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1)
    if [ -z "$out" ]; then
        failed "$cmd - version unknown" "$min_major"
        return
    fi
    major=${out%%.*}

    if [ "$major" -lt "$min_major" ]; then
        failed "$cmd - $out" "$min_major"
    else
        success "$cmd"
    fi
}

echo "${BOLD}Checking build dependencies${RESET}"
echo

commandIsAvailable "clang" 22
commandIsAvailable "ld.lld"
commandIsAvailable "llvm-ar"
commandIsAvailable "qemu-system-x86_64"
commandIsAvailable "uv"
commandIsAvailable "ninja"

echo
if [ "$fail_count" -eq 0 ]; then
    echo "${GREEN}${BOLD}All $total_count dependencies OK${RESET}"
else
    echo "${RED}${BOLD}$fail_count/$total_count dependencies missing or too old${RESET}"
    exit 1
fi
