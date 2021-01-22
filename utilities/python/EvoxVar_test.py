# This script is to generate source config file for Lobster
# It currently supports EvoxVar - 2016.3.15
#
# - Written by Steve Cai
# Copyright (C) 2015 by The Trane Company


from xml.etree import ElementTree
from xml.etree.ElementTree import Element
from xml.etree.ElementTree import SubElement
from xml.dom import minidom

import httplib

headers = {"Content-type": "xml"}
 
httpClient = httplib.HTTPConnection("192.168.1.3/evox/", 80, timeout=30)
httpClient.connect()

##httpClient.request('GET', '/evox/var')
##response = httpClient.getresponse()
##
##httpClient.request("POST", "/evox/var", "readAll", headers)
## 
##response = httpClient.getresponse()
##print response.status
##print response.reason
##print response.read()
##print response.getheaders()

            

def prettify(elem):
    # Return a pretty-printed XML string for the Element. #
    rough_string = ElementTree.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

##tree = ElementTree.ElementTree(file='VariablesObix.xml')
##root = tree.getroot()
##dataInfoList = root[1]
##print dataInfoList.attrib
##for dataInfo in dataInfoList:
##    print dataInfo.attrib['href']


##content = Element( 'obj', attrib={'is': 'Evox Var Config'} )
##listEle = SubElement( content, 'list', attrib={'is': 'Var Points'} )
##
##
##for varPoint in obj:
##    objEle = SubElement( listEle, 'obj', attrib={'is': 'Point'} )
##    SubElement(objEle, 'str', name='PointName', attrib={'val': varPoint[0]} )
##    SubElement(objEle, 'str', name='Href', attrib={'val': varPoint[1]} )
##    SubElement(objEle, 'str', name='ValueType', attrib={'val': varPoint[2]} )
##    SubElement(objEle, 'real', name='Slope', attrib={'val': varPoint[3]} )
##    SubElement(objEle, 'real', name='Offset', attrib={'val': varPoint[4]} )
##    SubElement(objEle, 'str', name='Authority', attrib={'val': varPoint[5]} )
##    SubElement(objEle, 'str', name='Nickname', attrib={'val': varPoint[6]} )
##
##
##output_file = open( 'EvoxVar_RTAF.xml', 'w' )
##output_file.write( prettify(content) )
##output_file.close()
