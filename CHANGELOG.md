# rdbus

# 0.3.0

- Added new data processing module - `"limits"`, for keeping track of minmax values in the given time period. Available for Modbus and Wago protocols.
- Added new configuration section - `"data_processors"` to enable data processing modules for given configuration.
- Added configuration settings for new `"wago"` (based on Ethernet Industrial Protocol) `"protocol"`. `rdbus` is now able to read analog data from Wago PLCs.
- Added new input type - TCP/IP (as in Internet Protocol), configurable with new `"address"` section, which is mutually exclusive with `"serial"`.

# 0.2.3
- Fixed 100% CPU usage bug.

# 0.2.2
- Added check to validate that only _ABCDEFGH_ letters can be present in `"data_order"`.

# 0.2.1
- `rdbus` now outputs an error if incoming NMEA data could not be parsed in any way (trash).
- Added check that validates that known `"data_type"` is not present with unnecessary `"data_order"`.

# 0.2.0
- Fixed `rdbus` not being able to parse NMEA messages which come in a single blob of continuous data.
- Added new optional `"metadata"` field in response JSON object. The field is not present in Modbus responses, but in case of NMEA contains sentence ID.
- Replaced config `"talker_id"` field of `"protocol": "nmea"` with `"name"`. The field is now used as arbitrary name of NMEA device.
- Sentence IDs can now be of any length. Previously the only acceptable incoming sentence IDs were at fixed length of 3.

# 0.1.0
- Defined `rdbus` API
