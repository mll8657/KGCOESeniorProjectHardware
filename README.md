# KGCOESeniorProjectHardware

Other EDCS repositories
[front end](https://github.com/Jss7268/KGCOESeniorProjectWeb)
[back end](https://github.com/Jss7268/KGCOESeniorProjectAPI)

A place to put various arduino code.

The upload_data script needs a particular file format. The first line has the device username and secret (password). The second line has the names of the values being measured starting with time. The third line has the units values are measured in, starting with milliseconds. Further lines all contain readings, the datapoints that are being collected. It looks like this:

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

# Instructions for converting upload_data.py into an exe file

For ease of use we decided to make a .exe version of the upload_data script. Anyone making changes to upload_data.py in the future will have to make a new .exe version as well. You can use pyinstaller to do so, following these steps.
### setting up python and pyinstaller (only need to do this once)
1. Get [Python 3](https://www.python.org/downloads/) installed on your machine. Check the "Add Python 3.x to PATH" option. (as of writing this, python 3.7.3 works fine)
2. Install the requests package to your Python environment by running `pip install requests` in your python terminal, or `python -m pip install requests` from a regular windows terminal.
3. From here, follow the appropriate section in the [manual for installing pyinstaller](https://pyinstaller.readthedocs.io/en/stable/installation.html) to install pyinstaller.
### making an exe file using pyinstaller (do this every time upload_data.py changes)
4. Open a terminal in the directory where upload_data.py is and run `pyinstaller -F upload_data.py` to create the upload_data.exe file
