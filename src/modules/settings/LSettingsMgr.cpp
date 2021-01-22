#include "LSettingsMgr.h"

#include <QMessageBox>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>


void writeSubTree(LObixTreeIter iter, QXmlStreamWriter &writer);
void writeStartElement(const LObixTreeIter &iter, QXmlStreamWriter &writer);


LSettingsMgr::LSettingsMgr()
{
    LObixObj obixObj("list", "is", "history");
    m_pSettingsTree = new LTree<LObixObj>(obixObj);
}

LSettingsMgr::~LSettingsMgr()
{
    if (m_pSettingsTree != NULL) {
        delete m_pSettingsTree, m_pSettingsTree = NULL;
    }
}

void LSettingsMgr::registerToSettingsMgr(LSettingsRelier *a_pSettingsRelier)
{
    // if you change th source code to add a new relier
    // please delete the old HistoryMemory.xml and let the program build a new one in case there might be some issues
    if(a_pSettingsRelier) {
        QString strRegisterName = a_pSettingsRelier->getRegisterName();

        if (!m_mapReliers.contains(strRegisterName)) {
            // add settings relier to map
            m_mapReliers[strRegisterName] = a_pSettingsRelier;

            // create the corresponding subtree
            LObixTreeMutableIter treeMutableIter(*m_pSettingsTree);
            treeMutableIter.moveToRoot();
            LObixObj obixObj("obj", "is", strRegisterName);
            treeMutableIter.addChild(obixObj);

            LObixTreeMutableIter subTreeIter(treeMutableIter);
            a_pSettingsRelier->setMutableIterator(subTreeIter);
        }
    }
}

void LSettingsMgr::unregisterToSettingsMgr(const QString &a_rstrRegisterName)
{
    if (m_mapReliers.contains(a_rstrRegisterName)) {
        // delete this subtree
        LObixTreeMutableIter treeIter(*m_pSettingsTree);
        treeIter.moveToRoot();
        if (treeIter.hasChild()) {
            treeIter.moveToFirstChild();
            if ( (treeIter.getValue().getType() == eObj) && (treeIter.getValue().getProperty("is") == a_rstrRegisterName) ) {
                treeIter.remove();
            }
            else {
                while(treeIter.hasSibling()) {
                    treeIter.moveToNextSibling();
                    if ( (treeIter.getValue().getType() == eObj) && (treeIter.getValue().getProperty("is") == a_rstrRegisterName) ) {
                        treeIter.remove();
                        break;
                    }
                }
            }
        }
        /// \note no need to delete relier pointer in case there is multiple inheritance
        ///       the module who owns the relier pointer has the responsibility to delete it
        m_mapReliers[a_rstrRegisterName] = NULL;
        m_mapReliers.remove(a_rstrRegisterName);
#if 0
        if (m_mapReliers[a_rstrRegisterName] != NULL) {
            delete m_mapReliers[a_rstrRegisterName], m_mapReliers[a_rstrRegisterName] = NULL;
        }
#endif
    }
}

void LSettingsMgr::readSettingsFile(const QString& a_rstrFilePath)
{
    #define START_ELEMENT   0
    #define END_ELEMENT     1
    unsigned int lastElement = START_ELEMENT;

    QFile file(a_rstrFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader reader(&file);
        LObixTreeMutableIter treeIter(*m_pSettingsTree);
        treeIter.moveToRoot();
        // build a new subtree
        if (treeIter.hasChild())
            treeIter.removeChildren();
        while (!reader.atEnd())
        {
            if(reader.isStartElement()) {
                LObixObj obixObjTmp;
                obixObjTmp.setType(reader.name().toString());
                if(!reader.attributes().isEmpty()) {
                    QVectorIterator<QXmlStreamAttribute> iter(reader.attributes());
                    while(iter.hasNext()) {
                        QXmlStreamAttribute attribute = iter.next();
                        obixObjTmp.addProperty(attribute.name().toString(), attribute.value().toString());
                    }
                }
                if (lastElement == START_ELEMENT) {
                    // if it's not root node
                    if ((obixObjTmp.getType() != eList) || (obixObjTmp.getProperty("is") != "history") ) {
                        treeIter.addChild(obixObjTmp);
                    }
                }
                else if(lastElement == END_ELEMENT) {
                    while(treeIter.hasSibling()) {
                        treeIter.moveToNextSibling();
                    }
                    treeIter.insertSibling(obixObjTmp);
                }
                lastElement = START_ELEMENT;
            }
            else if(reader.isEndElement()) {
                if (lastElement == END_ELEMENT) {
                    treeIter.moveToParent();
                }
                lastElement = END_ELEMENT;
            }

            reader.readNext();
        }
        file.close();

        //dispatch subtrees
        treeIter.moveToRoot();
        if (treeIter.hasChild()) {
            treeIter.moveToFirstChild();
            if(m_mapReliers.contains(treeIter.getValue().getProperty("is"))) {
                LObixTreeMutableIter subTreeMutableIter(treeIter);
                m_mapReliers[treeIter.getValue().getProperty("is")]->setMutableIterator(subTreeMutableIter);
                LObixTreeIter subTreeIter = m_pSettingsTree->constSubTree(treeIter);
                m_mapReliers[treeIter.getValue().getProperty("is")]->parseFromSetting(subTreeIter);
            }
            while(treeIter.hasSibling()) {
                treeIter.moveToNextSibling();
                if(m_mapReliers.contains(treeIter.getValue().getProperty("is"))) {
                    LObixTreeMutableIter subTreeMutableIter(treeIter);
                    m_mapReliers[treeIter.getValue().getProperty("is")]->setMutableIterator(subTreeMutableIter);
                    LObixTreeIter subTreeIter = m_pSettingsTree->constSubTree(treeIter);
                    m_mapReliers[treeIter.getValue().getProperty("is")]->parseFromSetting(subTreeIter);
                }
            }
        }
    }
#if 0
    else {
        // file doesn't exist, give the relier a mutable subtree to modify
        LObixTreeMutableIter treeMutableIter(*m_pSettingsTree);
        treeMutableIter.moveToRoot();
        if (treeMutableIter.hasChild()) {
            treeMutableIter.removeChildren();
        }
        QMapIterator<QString, LSettingsRelier*> i(m_mapReliers);
        while (i.hasNext()) {
            i.next();
            LObixObj obixObj("obj", "is", i.key());
            treeMutableIter.addChild(obixObj);
            LObixTreeMutableIter subTreeIter(treeMutableIter);
            if(i.value() != NULL) {
                i.value()->setMutableIterator(subTreeIter);
            }
            treeMutableIter.moveToParent();
        }
    }
#endif
}

void LSettingsMgr::writeSettingsFile(const QString& a_rstrFilePath)
{
    QFile file(a_rstrFilePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        // write start document
        writer.writeStartDocument();
        // write the whole tree
        LObixTreeIter treeIter = m_pSettingsTree->constRoot();
        treeIter.moveToRoot();
        writeSubTree(treeIter, writer);
        // write the end of document
        writer.writeEndDocument();
        file.close();
    }
    else
    {
        QMessageBox::warning(0,QObject::tr("Warning"), "can not write history files.\nYou may have permission problems");
    }
}

void writeSubTree(LObixTreeIter iter, QXmlStreamWriter &writer)
{
    if(iter.hasChild()) {
        writeStartElement(iter, writer);
        iter.moveToFirstChild();
        writeSubTree(iter, writer);

        while (iter.hasSibling()) {
            iter.moveToNextSibling();
            writeSubTree(iter, writer);
        }

        writer.writeEndElement();
    }
    else {
        writeStartElement(iter, writer);
        writer.writeEndElement();
    }
}

void writeStartElement(const LObixTreeIter &iter, QXmlStreamWriter &writer)
{
    writer.writeStartElement(iter.getValue().getTypeString());
    PropertyMap propertyMap = iter.getValue().getPropertyMap();
    QMapIterator<QString, QString> i(propertyMap);
    while(i.hasNext()) {
        i.next();
        writer.writeAttribute(i.key(), i.value());
    }
}
