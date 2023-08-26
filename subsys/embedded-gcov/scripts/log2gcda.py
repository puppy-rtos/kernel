import re
import os
import sys
# Read the log file
with open(sys.argv[1], "r") as f:
    log_data = f.read()

# split each file hexdump section
log_data.split(r":Emitting")
for split_data in log_data.split(r":Emitting"):

    # Get gcda file data section
    file_data = re.findall(r"(bytes for .+\.gcda)", split_data)
    if file_data == []:
        continue

    # Get gcda filename
    file_data = file_data[0].replace("bytes for ", "")
    # print("-------------")
    print(file_data)
    file_name = os.path.abspath(file_data)
    if os.path.exists(file_name):
        os.remove(file_name)
    # file_name = os.path.basename(file_data)

    # Find all hexdump sections
    hexdump_sections = re.findall(r"([0-9a-fA-F]{8}:\s[0-9a-fA-F\s]+\n)", split_data)
    # print(hexdump_sections)
    # Process each hexdump section
    for section in hexdump_sections:
        # Extract the hexdump data
        section = re.findall(r":(.*)", section)[0]
        hexdump_data = section.replace(" ", "")
        # Convert the hexdump data to bytes
        bytes_data = bytes.fromhex(hexdump_data)

        # Write the bytes data to a binary file
        with open(file_name, "ab") as f:
            f.write(bytes_data)

