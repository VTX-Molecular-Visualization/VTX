#!/bin/bash
set -u

cd "$(dirname "$0")/.."

VTX_CONAN_GRAPH_PATH="${VTX_CONAN_GRAPH_PATH:-dev}"
VTX_CONAN_REMOTE="${VTX_CONAN_REMOTE:-conancenter}"
GRAPH_JSON="$(mktemp)"

cleanup() {
    rm -f "$GRAPH_JSON"
}
trap cleanup EXIT

echo "Building Conan dependency graph from \"$VTX_CONAN_GRAPH_PATH\"..."
GRAPH_ARGS=()
if [ -n "${VTX_CONAN_GRAPH_ARGS:-}" ]; then
    read -r -a GRAPH_ARGS <<< "$VTX_CONAN_GRAPH_ARGS"
fi

if ! conan graph info "$VTX_CONAN_GRAPH_PATH" "${GRAPH_ARGS[@]}" --format=json > "$GRAPH_JSON"; then
    echo "Failed to build Conan graph."
    exit 1
fi

PYTHON_BIN="python3"
if ! command -v "$PYTHON_BIN" >/dev/null 2>&1; then
    PYTHON_BIN="python"
fi

"$PYTHON_BIN" - "$GRAPH_JSON" "$VTX_CONAN_REMOTE" <<'PY'
import json
import re
import subprocess
import sys

graph_path = sys.argv[1]
remote_name = sys.argv[2]

with open(graph_path, "r", encoding="utf-8") as graph_file:
    graph = json.load(graph_file)

nodes = graph.get("graph", {}).get("nodes", {})
root = nodes.get("0")
if not root:
    raise RuntimeError("Unable to find root node in Conan graph.")

refs = sorted(
    {
        dependency["ref"].split("#", 1)[0]
        for dependency in root.get("dependencies", {}).values()
        if dependency.get("direct") and dependency.get("ref")
    }
)

if not refs:
    raise RuntimeError("No direct dependency found in Conan graph.")


def version_scheme(version):
    if re.match(r"^cci\.\d+$", version):
        return "cci"
    if re.match(r"^\d{8}$", version):
        return "date"
    if re.match(r"^\d", version):
        return "numeric"
    return "other"


def version_tokens(version):
    return re.findall(r"\d+|[A-Za-z]+", version)


def compare_versions(left, right):
    if left == right:
        return 0

    left_tokens = version_tokens(left)
    right_tokens = version_tokens(right)
    count = max(len(left_tokens), len(right_tokens))

    for index in range(count):
        left_token = left_tokens[index] if index < len(left_tokens) else "0"
        right_token = right_tokens[index] if index < len(right_tokens) else "0"

        if left_token.isdigit() and right_token.isdigit():
            left_value = int(left_token)
            right_value = int(right_token)
            if left_value < right_value:
                return -1
            if left_value > right_value:
                return 1
            continue

        left_text = left_token.lower()
        right_text = right_token.lower()
        if left_text < right_text:
            return -1
        if left_text > right_text:
            return 1

    return 0


def list_versions(name):
    result = subprocess.run(
        ["conan", "list", f"{name}/*", f"-r={remote_name}", "--format=json"],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
    )

    if result.returncode != 0 or not result.stdout.strip():
        return None

    listing = json.loads(result.stdout)
    remote = next(iter(listing.values()), None)
    if not remote or "error" in remote:
        return None

    return [ref.split("/", 1)[1] for ref in remote.keys() if "/" in ref]


print(f"Checking {len(refs)} direct dependencies on {remote_name}...")
has_newer = False

for ref in refs:
    name, current = ref.split("/", 1)
    versions = list_versions(name)
    if versions is None:
        print(f"[?] {ref} -> list failed")
        continue

    current_scheme = version_scheme(current)
    compatible_versions = [
        version for version in versions if version_scheme(version) == current_scheme
    ]

    if not compatible_versions:
        print(f"[?] {ref} -> no compatible version")
        continue

    latest = current
    for version in compatible_versions:
        if compare_versions(latest, version) < 0:
            latest = version

    if latest != current:
        has_newer = True
        print(f"[+] {ref} -> {name}/{latest}")
    else:
        print(f"[=] {ref}")

sys.exit(2 if has_newer else 0)
PY
