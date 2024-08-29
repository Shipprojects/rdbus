# rdbus

- Added check that validates that known `"data_type"` is not present with unnecessary `"data_order"`.

# 0.2.0
- Fixed `rdbus` not being able to parse NMEA messages which come in a single blob of continuous data.
- Added new optional `"metadata"` field in response JSON object. The field is not present in Modbus responses, but in case of NMEA contains sentence ID.
- Replaced config `"talker_id"` field of `"protocol": "nmea"` with `"name"`. The field is now used as arbitrary name of NMEA device.
- Sentence IDs can now be of any length. Previously the only acceptable incoming sentence IDs were at fixed length of 3.

# 0.1.0
- Defined `rdbus` API
