#!/usr/bin/bash

trap 'cd /srv/comanage-registry/app && ./Console/cake job -U all > /proc/1/fd/1 2>&1' SIGTERM

while :
do
    /usr/bin/sleep infinity &
    pid="$!"
    wait "${pid}"
    break
done

kill "${pid}"
