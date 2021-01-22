import json
import csv

    
# open and parse csv file
with open('nti_config.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile)
    lineNum = 0
    obj = []    
    for line in reader:
        lineNum = lineNum + 1 
        if lineNum > 1: #skip the first line
            obj.append( line )

# transmit it to the json object
pyObj = {}
pointArray = []

for lineEle in obj:
    data = {}
    data["Name"] = lineEle[0]
    data["Address"] = lineEle[1]
    data["IsPU"] = lineEle[2]
    data["Authority"] = lineEle[3]
    data["Type"] = lineEle[4]
    pointArray.append(data)

pyObj["PointList"] = pointArray
pyObj["SlaveAddress"] = 0x01

jsonObj = json.dumps(pyObj, encoding="gbk", sort_keys=True, indent=4, separators=(',', ': '))
print(jsonObj)

with open('nti_config.json', 'w') as f:
    f.write(jsonObj)


# end of script
