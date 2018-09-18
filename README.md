# Install
```
sudo yum install systemd-devel gcc
gcc -o journal_perf -I/usr/include/ -lsystemd-journal journal_perf.c

# RHEL not supporting --user level systemd units (https://bugs.centos.org/view.php?id=8767#c28611) ...
sudo cp journal-*.service /run/systemd/system/
sudo systemctl daemon-reload

sudo mkdir -p /etc/systemd/journald.conf.d
cat <<EOF | sudo tee /etc/systemd/journald.conf.d/ratelimits.conf
[Journal]
MaxLevelConsole=warning
RateLimitInterval=1s
RateLimitBurst=100000
EOF
sudo systemctl restart systemd-journald
```

# Run with systemd
```
sudo systemctl start journald-perftest-syslog.service
sudo journalctl -u journald-perftest-stdout.service -n3
```

# Run manually
```
[centos@ip-172-31-39-103 ~]$ export UUID=$(uuidgen);echo $UUID; time ./journal_perf stdout $UUID 100000 | systemd-cat -t journal_perf;  journalctl -t journal_perf | grep "$UUID" | wc -l
ed6fb6f6-f93b-4b73-83f6-d98c912cc510

real	0m11.969s
user	0m0.031s
sys	0m0.007s
100004

[centos@ip-172-31-39-103 ~]$ export UUID=$(uuidgen);echo $UUID; time ./journal_perf journal $UUID 100000;  journalctl -t journal_perf | grep "$UUID" | wc -l
2307fbd0-c530-45cb-b107-58a2d645ef67

real	0m14.777s
user	0m0.165s
sys	0m0.397s
100000

[centos@ip-172-31-39-103 ~]$ export UUID=$(uuidgen);echo $UUID; time ./journal_perf syslog $UUID 100000;  journalctl -t journal_perf | grep "$UUID" | wc -l
d611289e-3756-4d65-89a2-de8a62a759b8

real	0m13.844s
user	0m0.162s
sys	0m0.085s
100000
```

# Run with [hyperfine](https://github.com/sharkdp/hyperfine)
```
curl https://sh.rustup.rs -sSf | sh
source $HOME/.cargo/env
cargo install hyperfine

[centos@ip-172-31-39-103 ~]$ export UUID=$(uuidgen);echo $UUID; hyperfine --prepare 'sync; echo 3 | sudo tee /proc/sys/vm/drop_caches' "./journal_perf stdout $UUID 100000 | systemd-cat -t journal_perf"
83f264e3-a8c8-4f33-81e8-3287eff394ba
Benchmark #1: ./journal_perf stdout 83f264e3-a8c8-4f33-81e8-3287eff394ba 100000 | systemd-cat -t journal_perf

  Time (mean ± σ):     11.685 s ±  0.237 s    [User: 32.5 ms, System: 6.0 ms]
  Range (min … max):   11.403 s … 12.088 s


[centos@ip-172-31-39-103 ~]$ export UUID=$(uuidgen);echo $UUID; hyperfine --prepare 'sync; echo 3 | sudo tee /proc/sys/vm/drop_caches' "./journal_perf syslog $UUID 100000"
7d1b58f0-2f7c-4d02-8a68-6831cc508064
Benchmark #1: ./journal_perf syslog 7d1b58f0-2f7c-4d02-8a68-6831cc508064 100000

  Time (mean ± σ):     13.884 s ±  0.053 s    [User: 152.5 ms, System: 97.0 ms]
  Range (min … max):   13.839 s … 14.002 s

[centos@ip-172-31-39-103 ~]$ export UUID=$(uuidgen);echo $UUID; hyperfine --prepare 'sync; echo 3 | sudo tee /proc/sys/vm/drop_caches' "./journal_perf journal $UUID 100000"
8b66e59f-1c9b-4053-b84b-2ebc7d09475e
Benchmark #1: ./journal_perf journal 8b66e59f-1c9b-4053-b84b-2ebc7d09475e 100000

  Time (mean ± σ):     14.878 s ±  0.037 s    [User: 189.5 ms, System: 401.3 ms]
  Range (min … max):   14.838 s … 14.950 s
```
