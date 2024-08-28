# ReadBus (rdbus)

Readbus (`rdbus`) is a program designed for serial data reading, data interpretation, and output. The program is designed to run on Little Endian processors regardless of cross-compilation capabilities. `rdbus` is a standalone executable and does not require any additional runtime libraries besides the system ones. It can read from multiple serial ports simultaneously.

# Table of contents

- [ReadBus (rdbus)](#readbus-rdbus)
- [Table of contents](#table-of-contents)
- [API](#api)
   * [Command line](#command-line)
   * [Configuration files](#configuration-files)
      + [Modbus](#modbus)
      + [NMEA](#nmea)
      + [Protocol](#protocol)
      + [Serial](#serial)
   * [Output](#output)
      + [HTTP server](#http-server)
      + [Stdout](#stdout)
   * [Stopping the progam](#stopping-the-progam)
- [Docker environment](#docker-environment)
- [Building](#building)
- [Running](#running)
- [Versioning](#versioning)

# API

`rdbus` was made with configurability in mind, so you can make it work in various different input/output combinations. The program requires some command line arguments and configuration file/-s. `rdbus` will not start with invalid configuration and will tell you what is wrong.

## Command line
Run
```bash
$ rdbus --help
```
to see command line arguments.

## Configuration files

The root of `rdbus` are `*.rdbus.json` configuration files. On startup, `rdbus` searches for them in current directory. You can also specify a different directory through the command line. Currently `rdbus` supports two input protocols - NMEA 0183 and Modbus RTU. The configuration for both protocols differs.

### Modbus

Example configuration for Modbus:
```json
{
    "protocol": "modbus",

    "serial": {
        "baud_rate": 9600,
        "parity": "none",
        "path": "/dev/ttyS1",
        "stop_bits_count": 1
    },

    "slaves": [
        {
            "id": 1,
            "name": "Torque meter",
            "poll_time_ms": 7000,
            "analog_input_registers": [
                {
                    "address": 0,
                    "data_type": "FLOAT_32A",
                    "name": "Power"
                }
            ],
            "analog_output_registers": [
                {
                    "address": 130,
                    "data_type": "UINT",
                    "data_order": "ABCD",
                    "name": "Torque"
                },
                {
                    "address": 134,
                    "data_type": "UINT_32A",
                    "name": "Shaft speed"
                }
            ]
        }
    ]
}
```
The Modbus specific part is `"slaves"`. Also note that `"protocol": "modbus"`. You can learn more about Modbus commands and responses [here](https://www.simplymodbus.ca/FAQ.htm#Command).

In `"slaves"` section, you must specify each Modbus slave that you want to read from.

| Field name                  | Description                                                                       |
|-----------------------------|-----------------------------------------------------------------------------------|
| `"id"`                      | Slave ID to request data from.                                                    |
| `"name"`                    | The name of slave device. Used for section identification in `rdbus` data output. |
| `"poll_time_ms"`            | A pause between each request in milliseconds.                                     |
| `"analog_input_registers"`  | Registers that can be accessed by using Modbus read request `0x03`.               |
| `"analog_output_registers"` | Registers that can be accessed by using Modbus read request `0x04`.               |

Now you must specify each register that you want to read. You can skip those registers in between that you do not want to - `rdbus` will take care of that.

| Field name     | Description                                                                                                            |
|----------------|------------------------------------------------------------------------------------------------------------------------|
| `"address"`    | Address of register. Note that a register consists of 2 bytes, but each consecutive address holds a separate register. I.e., registers 0 and 1 together take up 4 bytes|
| `"data_type"`  | Data type of register(-s).                                                                                             |
| `"data_order"` | Data order of wild data types (required with wild `"data_type"`-s only).                                                 |
| `"name"`       | The name of register. Used for field identification in `rdbus` data output.                                            |

#### Data order

There are two kinds of data types - _standard_ and _wild_. The only difference between them is that the former is already hardcoded in `rdbus` and does not require additional `"data_order"` field - just select the necessary `"data_type"`.

The data order describes where each of the bytes is stored in Modbus register/-s. It is described using up to 8 alphabetic letters - _ABCDEFGH_, where _A_ represents the most significant byte and _H_ the least significant. E.g., the combination:
```json
"data_type": "UINT",
"data_order": "CDAB",
```
describes a 2 consecutive register combination that is going to be interpreted as an unsigned integer with the described data order. You can put the letter combinations however you like.

#### Standard data types

| Data type     | Data order   |
|---------------|--------------|
| `"FLOAT_64A"` | `"ABCDEFGH"` |
| `"FLOAT_64B"` | `"HGFEDCBA"` |
| `"FLOAT_32A"` | `"ABCD"`     |
| `"FLOAT_32B"` | `"BADC"`     |
| `"FLOAT_32C"` | `"CDAB"`     |
| `"FLOAT_32D"` | `"DCBA"`     |
| `"UINT_64A"`  | `"ABCDEFGH"` |
| `"UINT_64B"`  | `"HGFEDCBA"` |
| `"UINT_32A"`  | `"ABCD"`     |
| `"UINT_32B"`  | `"BADC"`     |
| `"UINT_32C"`  | `"CDAB"`     |
| `"UINT_32D"`  | `"DCBA"`     |
| `"UINT_16A"`  | `"AB"`       |
| `"UINT_16B"`  | `"BA"`       |
| `"INT_64A"`   | `"ABCDEFGH"` |
| `"INT_64B"`   | `"HGFEDCBA"` |
| `"INT_32A"`   | `"ABCD"`     |
| `"INT_32B"`   | `"BADC"`     |
| `"INT_32C"`   | `"CDAB"`     |
| `"INT_32D"`   | `"DCBA"`     |
| `"INT_16A"`   | `"AB"`       |
| `"INT_16B"`   | `"BA"`       |

#### Wild data types

| Data type | Data order |
|-----------|------------|
| `"FLOAT"` | any        |
| `"INT"`   | any        |
| `"UINT"`  | any        |

### NMEA

Example configuration for NMEA:
```json
{
    "protocol": "nmea",

    "serial": {
        "baud_rate": 4800,
        "parity": "none",
        "path": "/dev/ttyS2",
        "stop_bits_count": 1
    },

    "checksum": false,
    "name": "NMEA device",
    "sentences": [
        {
            "id": "VHW",
            "fields": [
                {
                    "name": "Degress true",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Degrees true status",
                    "data_type": "STRING"
                },
                {
                    "name": "Degrees Magnetic",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Degrees Magnetic status",
                    "data_type": "STRING"
                },
                {
                    "name": "STW (knots/h)",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Knots status",
                    "data_type": "STRING"
                },
                {
                    "name": "STW (km/h)",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Kilometers status",
                    "data_type": "STRING"
                }
            ]
        },
        {
            "id": "VBW",
            "fields": [
                {
                    "name": "Longitudinal water speed",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Transverse water speed",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Water data valid",
                    "data_type": "STRING"
                },
                {
                    "name": "Longitudinal ground speed",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Transverse ground speed",
                    "data_type": "FLOAT"
                },
                {
                    "name": "Ground data valid",
                    "data_type": "STRING"
                }
            ]
        }
    ]
}
```
The NMEA specific part is `"checksum"`, `"name"` and `"sentences"`. Also note that `"protocol": "nmea"`. In case of NMEA, you will only have one device connected to a serial port.

| Field name    | Description                                                                                 |
|---------------|---------------------------------------------------------------------------------------------|
| `"checksum"`  | Either to check the checksum part of the incoming sentence or not.                          |
| `"name"`      | Arbitrary name of the device. Used for section identification in `rdbus` output.            |
| `"sentences"` | A list of all sentences that the talker is going to send.                                   |

Now you will have to describe the structure of all messages (sentences) that you are going to receive.

| Field name    | Description                                               |
|---------------|-----------------------------------------------------------|
| `"id"`        | Sentence identificator string.                            |
| `"fields"`    | All consecutive sentence fields.                          |

And each field in sentence.

| Field name    | Description                                                             |
|---------------|-------------------------------------------------------------------------|
| `"name"`      | The name of the field. Used for field identification in `rdbus` output. |
| `"data_type"` | Data type of the field.                                                 |

Allowed values for `"data_type"` are `"INT"`, `"UINT"`, `"FLOAT"`, `"STRING"`.

### Protocol

As was seen in previous examples, at the top of the json file we have the `"protocol"` field.
```json
{
    "protocol": "modbus",
    ...
```
The two allowed values are `"modbus"` and `"nmea"`. Depending on which you select your config will be parsed accordingly.

### Serial

Each config file is meant to operate with one serial port. In each config file there must be a `"serial"` section.
```json
{
...
    "serial": {
        "baud_rate": 4800,
        "parity": "none",
        "path": "/dev/ttyS2",
        "stop_bits_count": 1,
        "response_timeout_ms": 500,
        "line_timeout_ms": 10,
    },
...
}
```

| Field name              | Description                                                     |
|-------------------------|-----------------------------------------------------------------|
| `"baud_rate"`           | Baud rate of serial port.                                       |
| `"parity"`              | Parity bit setting. Can be either `"none"`, `"even"` or `"odd"` |
| `"path"`                | Path to the device file of serial port i.e.`"/dev/*"`.          |
| `"stop_bits_count"`     | Either `1` or `2`.                                              |
| `"response_timeout_ms"` | Max time to wait for data to arrive.                            |
| `"line_timeout_ms"`     | Max time to wait for each data segment (8 bytes) to arrive. You can calculate the minimum time (in milliseconds) by using formula `y=8000/(baud_rate/8)`. It is strongly recommended to round the result up, e.g. 6.6ms to 10ms, 13ms to 20ms, etc.|

## Output

`rdbus` can output data in 2 ways - either by stdout or by HTTP server. You can select the desired output type using command line arguments. The output is a list of JSON objects whenever it gets available (read). There may be data present from different devices in a single list. You can see which device the data comes from by checking the `"device"` field. Example output:

```json
[
    {
        "device": "Torque meter",
        "fields": [
            {
                "name": "Torque",
                "timestamp": "2024-08-22 22:50:12:456",
                "value": 1242844016
            },
            {
                "name": "Shaft speed",
                "timestamp": "2024-08-22 22:50:12:456",
                "value": 1242844016
            },
            {
                "name": "Power",
                "timestamp": "2024-08-22 22:50:12:456",
                "value": 2429660.0
            }
        ]
    },
    {
        "device": "Flow meter",
        "error": {
            "code": 1,
            "what": "OS exception - Timeout - Device timeout!"
        }
    },
    {
        "device": "Flow meter",
        "fields": [
            {
                "name": "Mass Flow",
                "timestamp": "2024-08-22 22:50:17:581",
                "value": 2415391.0
            },
            {
                "name": "Volume flow",
                "timestamp": "2024-08-22 22:50:17:875",
                "value": 2406476.0
            }
        ]
    }
]
```
where a single entry can consist of either

| Field name | Description                                                             |
|------------|-------------------------------------------------------------------------|
| `"device"` | Slave `"name"` in case of Modbus or top level `"name"` in case of NMEA. |
| `"fields"` | A list of data fields.                                                  |
| `"metadata"` | An optional helper field. Currently not present in Modbus, but in case of NMEA contains sentence id so you could have sentences with the same field names but different meanings. |

or

| Field name | Description                                                        |
|------------|--------------------------------------------------------------------|
| `"device"` | Slave `"name"` in case of Modbus or top level `"name"` in case of NMEA. |
| `"error"`  | Error description object.                                          |

Structure of a single entry in `"fields"`:

| Field name    | Description                                                                                                     |
|---------------|-----------------------------------------------------------------------------------------------------------------|
| `"name"`      | Register `"name"` in case of Modbus or sentence field `"name"` in case of NMEA.                                 |
| `"timestamp"` | Timestamp of when the reading was made. Note that in a single list of fields there may be different timestamps. |
| `"value"`     | The value of field.                                                                                             |

Structure of an `"error"`:

| Field name | Description                                                                                                                                       |
|------------|---------------------------------------------------------------------------------------------------------------------------------------------------|
| `"code"`   | `1` - OS exception (e.g. could not read from port). `2` - Modbus error (e.g. invalid message). `3` - NMEA error (e.g. invalid checksum). |
| `"what"`   | Description of an error.

### HTTP server

If you have passed `--ip` parameter to `rdbus` it starts an HTTP server on given address. To retrieve data you have to perform an HTTP request. For example:
```bash
$ curl --verbose http://0.0.0.0:5000/read?pretty
```

#### `/read` request

The `/read` request asks `rdbus` for all it's buffered data.

| Parameters | Description              |
|------------|--------------------------|
| `"pretty"` | Output indented json.    |

The server buffers all data read from serial ports up to a hardcoded limit of 500 KB, after reaching it, the buffer will start to delete the oldest elements. Note that the server stores data in binary format, which takes up much less space than the JSON that it outputs, meaning that the 500 KB of internally buffered data means several MB of JSON output.

You will probably want to read all buffered data only once - when you perform the first request, after which you will be interested only in reading new data. To achieve this, `rdbus` uses cookies. When you send a `/read` request without a cookie, the server will start a session for you which expires after 5 minutes and gets extended each time you `/read`. For example:
```bash
$ curl --verbose http://0.0.0.0:5000/read?pretty
*   Trying 0.0.0.0:5000...
* Connected to 0.0.0.0 (127.0.0.1) port 5000 (#0)
> GET /read?pretty HTTP/1.1
> Host: 0.0.0.0:5000
> User-Agent: curl/7.81.0
> Accept: */*
>
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Content-Length: 942
< Content-Type: text/plain
< Keep-Alive: timeout=5, max=5
< Set-Cookie: 1
<
[
    {
        "device": "Torque meter",
        "registers": [
            {
                "name": "Torque",
                "timestamp": "2024-08-23 12:40:28:851",
                "value": 1243234916
            },
            {
                "name": "Shaft speed",
                "timestamp": "2024-08-23 12:40:28:851",
                "value": 1243234916
            },
            {
                "name": "Power",
                "timestamp": "2024-08-23 12:40:28:851",
                "value": 2527385.0
            }
        ]
    },
    {
        "device": "Flow meter",
        "registers": [
            {
                "name": "Mass Flow",
                "timestamp": "2024-08-23 12:40:28:968",
                "value": 2513104.0
            },
            {
                "name": "Volume flow",
                "timestamp": "2024-08-23 12:40:29:262",
                "value": 2504189.0
            }
        ]
    }
* Connection #0 to host 0.0.0.0 left intact
]

$ curl -v --cookie "id=1" http://0.0.0.0:5000/read?pretty
*   Trying 0.0.0.0:5000...
* Connected to 0.0.0.0 (127.0.0.1) port 5000 (#0)
> GET /read?pretty HTTP/1.1
> Host: 0.0.0.0:5000
> User-Agent: curl/7.81.0
> Accept: */*
> Cookie: id=1
>
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Content-Length: 395
< Content-Type: text/plain
< Keep-Alive: timeout=5, max=5
<
[
    {
        "device": "Flow meter",
        "registers": [
            {
                "name": "Mass Flow",
                "timestamp": "2024-08-23 12:40:33:990",
                "value": 2513114.0
            },
            {
                "name": "Volume flow",
                "timestamp": "2024-08-23 12:40:34:287",
                "value": 2504198.0
            }
        ]
    }
* Connection #0 to host 0.0.0.0 left intact
]
```
As you can see, there is a `Set-Cookie: 1` field present in first response header. If you send this cookie back on the next request in format `id=<id>` then you will receive only the new data that you have not read yet.

### Stdout

In case of stdout output, each time the data is read, it will be immediately outputted to `stdout`. For example:
```bash
$ ./rdbus --stdout --log info
[2024-08-23 12:48:38:971] [info] [239494] Starting
[2024-08-23 12:48:38:971] [info] [239494] Outputting to STDOUT
[2024-08-23 12:48:38:972] [info] [239495] Starting manager of /dev/ttyS1
[2024-08-23 12:48:38:972] [info] [239496] Starting manager of /dev/ttyS0
[2024-08-23 12:48:38:972] [info] [239496] Sending data 01040000000271cb
[2024-08-23 12:48:38:972] [info] [239495] Sending data 01030082000665e0
[2024-08-23 12:48:39:148] [info] [239495] Data received 01030c4a1a516c4a1a516c4a1a516cf8ad
[
    {
        "device": "Torque meter",
        "registers": [
            {
                "name": "Torque",
                "timestamp": "2024-08-23 12:48:39:148",
                "value": 1243238764
            },
            {
                "name": "Shaft speed",
                "timestamp": "2024-08-23 12:48:39:148",
                "value": 1243238764
            },
            {
                "name": "Power",
                "timestamp": "2024-08-23 12:48:39:148",
                "value": 2528347.0
            }
        ]
    }
]
[2024-08-23 12:48:39:265] [info] [239496] Data received 0104044a19724818cd
[2024-08-23 12:48:39:266] [info] [239496] Sending data 01040006000291ca
[2024-08-23 12:48:39:564] [info] [239496] Data received 0104044a18e6fc27ba
[
    {
        "device": "Flow meter",
        "registers": [
            {
                "name": "Mass Flow",
                "timestamp": "2024-08-23 12:48:39:266",
                "value": 2514066.0
            },
            {
                "name": "Volume flow",
                "timestamp": "2024-08-23 12:48:39:564",
                "value": 2505151.0
            }
        ]
    }
]
```
Note that the logs get outputted to `stderr`.

## Stopping the progam

The program exits when it receives a `SIGINT`. You can either do it from the command line by pressing `Ctrl+C` or by
```bash
$ kill -s SIGINT <rdbus_process_id>
```

`0` return code means no errors. `1` means that something went wrong during execution.

# Docker environment

There is a Docker image available with everything you need to develop and build `rdbus`. Follow [this guide](https://docs.docker.com/engine/install/ubuntu/) to install Docker and [this guide](https://docs.docker.com/engine/install/linux-postinstall/#manage-docker-as-a-non-root-user) to finalize your Docker setup. For development you will also need Visual Studio Code with `ms-vscode-remote.remote-containers` extension installed and `~/.ssh` directory present on your machine that will be mounted in Docker container.

To open the development environment you have to open the `rdbus` project root directory in `vscode` and run `>Dev Containers: Reopen in Container`. Wait for the image to build for the first time and then wait until all extensions get automatically installed. Now you are ready to go.

This README file does not go into further details on how to develop the program or use the environment. Just know that this setup has all the necessary tools installed and gets automatically configured.

# Building

There are 2 main targets that you can build - `rdbus` and `rdbus_tests`. The latter is a unit testing executable that is tied to the environment. Before building, you have to perform CMake configure once:
```bash
$ cmake --preset Release
```

Now if you want to build everything:
```bash
$ cmake --build --preset Release
```

If you want to build specific executables:
```bash
$ cmake --build --preset Release --target rdbus
```
or
```bash
$ cmake --build --preset Release --target rdbus_tests
```

The binaries are located in `build/Release/src/apps/` and `build/Release/tests/` respectively.

# Running

"But.. but I can't run it without Modbus/NMEA devices connected to my machine!". That is valid thought, until we remember that we use superior operating system - Linux. You can use `rdbus` locally without any additional ports and devices attached to your machine using `launch.py` script. It starts `socat` serial port emulation along with preconfigured Modbus slave and `rdbus` that is located in your `build/Release` directory.

```bash
$ ./launch.py --stdout
```
starts `rdbus` in stdout mode with the necessary environment.

The Docker environment contains two executables - `mbclient` and `mbserver` which are Modbus Master and Slave executables respectively. `launch.py` uses `mbserver`. You can also use `mbserver` in GUI mode and check Modbus values in the app.
```bash
$ mbserver -project run.config/mbserver.pjs
```
In such case you can
```bash
$ ./launch.py --no-slave --stdout
```
to launch `rdbus` without additional `mbserver` instance and use the one you just opened in GUI.

# Versioning

The versioning of this project adheres to [Semantic Versioning 2.0.0](https://semver.org/) specification. In summary:
> Given a version number MAJOR.MINOR.PATCH, increment the:
>
>     MAJOR version when you make incompatible API changes
>     MINOR version when you add functionality in a backward compatible manner
>     PATCH version when you make backward compatible bug fixes
