#!/bin/bash
#
# Test the image tags created by container/build.sh --cilogon-ecr without
# requiring docker. A fake docker placed first on PATH records every
# invocation, and the assertions below inspect that record.
#
# Usage: container/test/build_cilogon_ecr_test.sh

set -u

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
LABEL="testlabel"
SUFFIX="1"

REGISTRY_TAG="comanage-registry:${LABEL}-mod_auth_openidc-${SUFFIX}"
CROND_TAG="comanage-registry-cron:${LABEL}-${SUFFIX}"
PUBLIC_ECR="public.ecr.aws/cilogon/comanage-registry"
GHCR="ghcr.io/cilogon/comanage-registry"
PRIVATE_ECRS=(
    "495649616520.dkr.ecr.us-east-2.amazonaws.com"
    "495649616520.dkr.ecr.ap-southeast-2.amazonaws.com"
)

failures=0

###########################################################################
# Create a fake docker that logs each invocation and exits non-zero only
# for a tag command whose target equals DOCKER_FAIL_TAG_TARGET.
# Globals:
#   FAKE_BIN, DOCKER_LOG
# Arguments:
#   None
# Outputs:
#   None
###########################################################################
function setup_fake_docker() {
    FAKE_BIN="$(mktemp -d)"
    DOCKER_LOG="${FAKE_BIN}/docker.log"

    cat > "${FAKE_BIN}/docker" <<'EOF'
#!/bin/bash
echo "$*" >> "${DOCKER_LOG}"
if [[ "$1" == "tag" && -n "${DOCKER_FAIL_TAG_TARGET:-}" \
      && "$3" == "${DOCKER_FAIL_TAG_TARGET}" ]]; then
    exit 1
fi
exit 0
EOF
    chmod +x "${FAKE_BIN}/docker"
}

###########################################################################
# Run build.sh with the fake docker, logging to a fresh DOCKER_LOG.
# Globals:
#   FAKE_BIN, DOCKER_LOG, REPO_ROOT
# Arguments:
#   Arguments for build.sh.
# Outputs:
#   Returns the exit status of build.sh.
###########################################################################
function run_build() {
    : > "${DOCKER_LOG}"
    (cd "${REPO_ROOT}" \
        && PATH="${FAKE_BIN}:${PATH}" DOCKER_LOG="${DOCKER_LOG}" \
           bash container/build.sh "$@" > /dev/null 2>&1)
}

###########################################################################
# Record a pass or fail line for one scenario.
# Globals:
#   failures
# Arguments:
#   Scenario description, a string.
#   Result, 0 for pass and anything else for fail.
# Outputs:
#   Writes PASS or FAIL line to stdout.
###########################################################################
function check() {
    if [[ "$2" == 0 ]]; then
        echo "PASS: $1"
    else
        echo "FAIL: $1"
        failures=$((failures + 1))
    fi
}

###########################################################################
# Return 0 if the docker log contains the exact line.
# Globals:
#   DOCKER_LOG
# Arguments:
#   Expected line, a string.
###########################################################################
function logged() {
    grep -Fxq -- "$1" "${DOCKER_LOG}"
}

setup_fake_docker
trap 'rm -rf "${FAKE_BIN}"' EXIT

run_build --label "${LABEL}" -s "${SUFFIX}" --cilogon-ecr
check "--cilogon-ecr run exits 0" $?

logged "tag ${REGISTRY_TAG} ${PUBLIC_ECR}:${LABEL}-mod_auth_openidc-${SUFFIX}"
check "registry image tagged for public ECR with versioned tag" $?

logged "tag ${REGISTRY_TAG} ${PUBLIC_ECR}:latest"
check "registry image tagged for public ECR with :latest" $?

logged "tag ${REGISTRY_TAG} ${GHCR}:latest"
check "registry image tagged for GHCR with :latest" $?

[[ "$(grep -c ' ghcr\.io/' "${DOCKER_LOG}")" == 1 ]]
check "GHCR receives exactly one tag" $?

result=0
for ecr in "${PRIVATE_ECRS[@]}"; do
    logged "tag ${REGISTRY_TAG} ${ecr}/${REGISTRY_TAG}" || result=1
    logged "tag ${CROND_TAG} ${ecr}/${CROND_TAG}" || result=1
done
check "private regional ECR tags unchanged for both images" ${result}

! grep -Eq '^tag comanage-registry-cron:[^ ]+ (public\.ecr\.aws|ghcr\.io)/' \
    "${DOCKER_LOG}"
check "cron image not tagged for public repositories" $?

! grep -Eq '^tag [^ ]+ [^ ]*dkr\.ecr[^ ]*:latest$' "${DOCKER_LOG}"
check "no :latest tag for private regional ECRs" $?

! grep -Eq '^(image )?push( |$)| --push( |$)' "${DOCKER_LOG}"
check "no docker push" $?

for target in "${PUBLIC_ECR}:${LABEL}-mod_auth_openidc-${SUFFIX}" \
              "${PUBLIC_ECR}:latest" \
              "${GHCR}:latest"; do
    DOCKER_FAIL_TAG_TARGET="${target}" \
        run_build --label "${LABEL}" -s "${SUFFIX}" --cilogon-ecr
    status=$?
    [[ ${status} != 0 ]]
    check "failed tag of ${target} makes build.sh exit non-zero" $?
done

help="$(cd "${REPO_ROOT}" && bash container/build.sh --help 2>&1)"
[[ "${help}" == *"${PUBLIC_ECR}"* && "${help}" == *"${GHCR}"* ]]
check "--help names both public repositories" $?

if ((failures > 0)); then
    echo "${failures} scenario(s) failed"
    exit 1
fi

echo "All scenarios passed"
