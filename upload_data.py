import json
import requests
import sys
print("KGCOE senior project data upload script.")
print("See readme.txt or https://github.com/mll8657/KGCOESeniorProjectHardware for description of valid data files")


def main():
    try:
        data_file = open(sys.argv[1], "r")
    except IndexError:
        print("no file supplied. Please drag valid data file on to upload_data.exe")
        return
    except FileNotFoundError:
        print("Cannot find supplied file. Possibly a permissions issue")
        return

    # First line has device name & secret
    # ex: test_device,test_secret
    line1 = data_file.readline().strip().split(",")
    if len(line1) != 2:
        print("invalid first line")
        print("The first line of your file should have 2 values: a device name and device secret (password)")
        print("ex: device_name,device_secret")
        return
    device_name = line1[0]
    device_secret = line1[1]

    # Second line has names for the readings
    # ex: time,rotations,temperature...
    value_names = data_file.readline().strip().split(",")
    if len(value_names) <= 1:
        print("Could not read value names")
        print("The second line of your file should have a list of value names starting with time")
        print("ex: time,temperature,pressure...")
        return
    skip_warning = False
    for name in value_names:
        try:
            if float(name) and not skip_warning:
                print("WARNING: name \""+name+"\" in line 2 appears to be a number. Line 2 is for value names, which typically have letters in them.")
                input("If the value name is correct, press enter to ignore warning and continue")
                skip_warning = True
        except ValueError:
            pass

    # Third line has units for the readings
    # ex: seconds, rotations, degrees F...
    value_units = data_file.readline().strip().split(",")
    if len(value_units) != len(value_names):
        print("number of types inconsistent with units")
        print(str(len(value_units)) + " units were specified, but " + str(len(value_names)) + " value types were named")
    skip_warning = False
    for unit in value_units:
        try:
            if float(unit) and not skip_warning:
                print("WARNING: unit \""+unit+"\" in line 3 appears to be a number. Line 3 is for unit names, which typically have letters in them.")
                input("If the unit name is correct, press enter to ignore warning and continue")
                skip_warning = True
        except ValueError:
            pass

    # remaining lines are all device outputs to be uploaded
    line_number = 3
    validated_lines = []
    for line in data_file.readlines():
        reading = line.strip().split(",")
        line_number += 1
        if len(reading) != len(value_names):
            print("data inconsistent with number of values")
            print("Line number "+str(line_number)+" has "+str(len(reading))+" values, but "+str(len(value_names))+" values were named")
            return
        validated_lines.append(reading)

    # all lines read/validated, begin making requests
    # try to authenticate
    auth_req = requests.post("http://localhost:8080/api/auth/authenticate", json={"email": device_name, "password": device_secret})
    if auth_req.status_code != 200:
        print("device authentication failed")
        print("make sure the first line of the file contains only the device name and secret")
        print("ex: device_name,secret")
        return
    try:
        auth_token = json.loads(auth_req.content)['token']
    except IndexError:
        # shouldn't happen. successful authentications should always give a token
        print("authentication request succeeded but no token was attached, cannot authenticate.")
        return

    # create output types for the data about to be stored
    for i in range(0, len(value_names)):
        output_type_req = requests.post("http://localhost:8080/api/v1/output_types", json={"output_type_name": value_names[i], "units": value_units[i]},
                                        headers={'Authorization': 'Bearer '+auth_token})
        # duplicate key gives a 400 error, which is also okay because that just means the type already exists
        if output_type_req.status_code == 201 or output_type_req.status_code == 400:
            pass
        else:
            print("unexpected response code "+str(output_type_req.status_code))
            print("response content: "+str(output_type_req.content))
            return

    # get the device id
    device_info = requests.get("http://localhost:8080/api/v1/users/me", headers={'Authorization': 'Bearer '+auth_token})
    if device_info.status_code != 200:
        # shouldn't happen, if you're authenticated you should be able to get your own info
        print("unable to get device info, probably your internet connection or the server is down.")
        return
    device_id = json.loads(device_info.content)['id']

    # We decided the data should just go to the last experiment the device was linked to.
    # This code still works though, might as well keep it
    #
    # Make a new experiment to put device outputs in
    # experiment_req = requests.post("http://localhost:8080/api/v1/experiments",
    #                                headers={'Authorization': 'Bearer '+auth_token},
    #                                json={
    #                                    "creator_id": device_id,
    #                                    "description": "auto generated experiment from "+json.loads(device_info.content)['email']
    #                                })
    # if experiment_req.status_code != 201:
    #     print("unable to create experiment")
    #     print("response content: "+str(experiment_req.content))
    #     return
    # experiment_id = json.loads(experiment_req.content)['id']

    # finally, create device outputs
    for line in validated_lines:
        for i in range(1, len(line)):
            device_output_req = requests.post("http://localhost:8080/api/v1/device_outputs",
                                              headers={'Authorization': 'Bearer '+auth_token},
                                              json={
                                                "output_type_name": value_names[i],
                                                "device_id": device_id,
                                                "output_value": line[i],
                                                "timestamp": line[0],
                                                # "experiment_id": experiment_id # only needed if making new experiment
                                              })
            if device_output_req.status_code != 201:
                print("unexpected response code: "+str(device_output_req.status_code))
                print("response content: "+str(device_output_req.content))
                return

    data_file.close()
    print("done!")
    return


main()
while True:
    pass