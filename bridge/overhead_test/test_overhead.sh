 #!/usr/bin/env bash
  set -euo pipefail

  DURATION="${1:-60}"

  PIDS="$(
    ps -eo pid,cmd \
      | grep -E "ros_gz_bridge|parameter_bridge|bridge_node" \
      | grep -v grep \
      | awk '{print $1}' \
      | paste -sd, -
  )"

  if [ -z "$PIDS" ]; then
    echo "No bridge process found."
    exit 1
  fi

  PID_COUNT="$(echo "$PIDS" | tr ',' '\n' | wc -l)"

  echo "Bridge PIDs: $PIDS"
  echo "Sampling $DURATION seconds..."

  for i in $(seq 1 "$DURATION"); do
    printf "\rProgress: %d/%d" "$i" "$DURATION" >&2
    ps -p "$PIDS" -o %cpu=,rss=
    sleep 1
  done | awk -v pid_count="$PID_COUNT" '
  {
    sample_cpu += $1
    sample_rss += $2
    count++

    if (count == pid_count) {
      total_cpu += sample_cpu
      total_rss += sample_rss
      samples++

      sample_cpu = 0
      sample_rss = 0
      count = 0
    }
  }
  END {
    printf "\n"
    printf "avg_total_cpu=%.2f%%\n", total_cpu / samples
    printf "avg_total_rss=%.1f MB\n", total_rss / samples / 1024
    printf "samples=%d\n", samples
  }
  '

  echo >&2