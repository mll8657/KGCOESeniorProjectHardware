# KGCOESeniorProjectHardware

A place to put various arduino code.

The current upload_data script needs a particular file format. The first line has the device username and secret (password). The second line has the names of the values being measured starting with time. The third line has the units values are measured in, starting with milliseconds. Further lines all contain readings, the datapoints that are being collected. It looks like this:

```
username,password
time,value1_name,value2_name,value3_name...
milliseconds,value1_units,value2_units,value3_units...
time_reading,value1_reading,value2_reading,value3_reading...
...
```

a more concrete example would be:

```
d@test.com,password
time,wheel_rotations,rotation_rate
milliseconds,cycles,hertz
100,0.0,0.00
200,0.1,0.95
```
