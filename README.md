# Zabbix Module for Sysrepo Monitoring

[Sysrepo](https://www.sysrepo.org/) is a YANG-based system repository for
all-around configuration management. This loadable module enables the native
[Zabbix agent](https://www.zabbix.com/zabbix_agent) to monitor arbitrary
datastore items.

## Templates

We offer [templates](templates) with most common items. Import the template to
the Zabbix server and link it with your device.

## Available Keys

You can create custom items using the following keys:

| Key                    | Description |
| ---------------------- | ----------- |
| sysrepo.get[\<xpath\>] | Retrieve a value from the operational store. |

For an XPath with square brackets use double quotes, e.g.:
`sysrepo.get["/ietf-hardware:hardware/component[name='hwmon0/temp1']/sensor-data/value"]`.

You can test the module using:
```bash
zabbix_agentd -t sysrepo.get[/ietf-system:system-state/platform/os-name]
zabbix_agentd -t 'sysrepo.get["/ietf-hardware:hardware/component[name=\"hwmon1/in0\"]/sensor-data/value"]'
```

## Installation

Download [Zabbix sources](https://www.zabbix.com/download_sources) of the version
you got installed.

Build the module using:
```bash
mkdir build; cd build
PKG_CONFIG_PATH=/usr/lib/pkgconfig/ cmake -DCMAKE_INSTALL_PREFIX=/usr -DZABBIX_DIR=/home/user/zabbix-5.0.29 ..
make
make install
```

Make sure the the `/etc/zabbix_agentd.conf` includes
```
Include=/etc/zabbix_agentd.conf.d/*.conf
```

## License

This module is made available under the terms of the
[GNU General Public License v2.0 or later](LICENSE).
