# This script is to generate source config file for Lobster
# It currently supports EvoxVar - 2015.12.23
#
# - Written by Steve Cai
# Copyright (C) 2015 by The Trane Company


import csv

    

with open('EvoxVar_Quantum.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile)
    lineNum = 0
    obj = []    
    for line in reader:
        lineNum = lineNum + 1 
        if lineNum > 1:
            obj.append( line )

            
            
from xml.etree import ElementTree
from xml.etree.ElementTree import Element
from xml.etree.ElementTree import SubElement
from xml.dom import minidom

def prettify(elem):
    # Return a pretty-printed XML string for the Element. #
    rough_string = ElementTree.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")


content = Element( 'obj', attrib={'is': 'Evox Var Config'} )
listEle = SubElement( content, 'list', attrib={'is': 'Var Points'} )


for varPoint in obj:
    objEle = SubElement( listEle, 'obj', attrib={'is': 'Point'} )
    SubElement(objEle, 'str', name='PointName', attrib={'val': varPoint[0]} )
    if varPoint[1] != "Auto" :  # Auto is default
        SubElement(objEle, 'str', name='Href', attrib={'val': varPoint[1]} )
    if varPoint[2] != "real" :  # real is default
        SubElement(objEle, 'str', name='ValueType', attrib={'val': varPoint[2]} )
    if varPoint[3] != "1" :     # 1 is default
        SubElement(objEle, 'real', name='Slope', attrib={'val': varPoint[3]} )
    if varPoint[4] != "0" :     # 0 is default
        SubElement(objEle, 'real', name='Offset', attrib={'val': varPoint[4]} )
    if varPoint[5] != "ReadWrite":  # ReadWrite is default
        SubElement(objEle, 'str', name='Authority', attrib={'val': varPoint[5]} )
    if varPoint[6] != "NULL" :  # NULL stands for not nick name
        SubElement(objEle, 'str', name='Nickname', attrib={'val': varPoint[6]} )


output_file = open( 'EvoxVar_Quantum.xml', 'w' )
output_file.write( prettify(content) )
output_file.close()
