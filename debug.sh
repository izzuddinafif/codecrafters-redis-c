echo -e "for the client, run \x1b[34mredis-cli -p 6380\x1b[0m"
socat -v TCP-LISTEN:6380,fork TCP:localhost:6379 2>&1 | sed '
  s/^>.*$/\x1b[35m&\x1b[0m/;     # magenta
  s/^<.*$/\x1b[36m&\x1b[0m/;     # cyan
  s/\*[0-9]*/\x1b[34m&\x1b[0m/g; # blue
  s/\$[0-9]*/\x1b[33m&\x1b[0m/g; # yellow
  s/^+.*$/\x1b[32m&\x1b[0m/;     # green
  s/^-.*$/\x1b[31m&\x1b[0m/;     # red
'
