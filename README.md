iBUS engine for varnam
======================

Installation
------------

Installing dependencies on Debian:

```shell
sudo apt-get install libibus-1.0-dev libcurl4-openssl-dev libgtk-3-dev
```

Installing iBus engine:

```shell
git clone https://github.com/varnamproject/libvarnam-ibus.git && cd libvarnam-ibus
cmake . -DVARNAM_LANGUAGES="ml;ml-inscript;hi" && make && sudo make install
ibus restart
```
where the variable `VARNAM_LANGUAGES` is a semicolon-separated list of languages varnam was compiled with.

Usage
-----

### Ubuntu

-	Choose `Text Entry Settings` from `All Settings`
-	Add a new input source
-	Type the language required and choose Varnam from it
-	Varnam icon should now be available in the language bar. Click on it to activate varnam input method

![Choose input source](https://raw.githubusercontent.com/varnamproject/libvarnam-ibus/master/ibus-usage1.png) ![Choose input source](https://raw.githubusercontent.com/varnamproject/libvarnam-ibus/master/ibus-usage.png)

Troubleshooting
---------------

Logs will be available at `$HOME/.config/varnam/ibus-engine/logs`
