# Torrent Chaset

Tiny torrent client with monitoring bittorrent network ip.


# CODE LICENSE

- httplib.h
  https://github.com/yhirose/cpp-httplib.git

- nlohmann/json.hpp
  https://github.com/nlohmann/json

- sqllite
sqlite-autoconf-3340100.tar.gz
https://www.sqlite.org/download.html
```
$tar xvfz sqlite-autoconf-3071502.tar.gz
$cd sqlite-autoconf-3071502
$./configure --prefix=/usr/local
$make
$make install
```

- libtorrent 
https://github.com/arvidn/libtorrent.git (push)
```
root@118-27-105-203:/app/libtorrent# git log
commit 38738795c02325bcb09ba7749a8be9e9b0496c61 (HEAD -> RC_2_0, origin/RC_2_0, origin/HEAD)
Author: arvidn <arvid@libtorrent.org>
Date:   Fri Jan 22 02:29:14 2021 +0100

    add std::hash<> specialization for info_hash_t
```