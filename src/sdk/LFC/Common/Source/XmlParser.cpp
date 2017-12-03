/*
 * XmlParser.cpp
 *
 *  Created on: Jun 01, 2016
 *      Author: lory
 */

#include <stdlib.h>
#include "../Include/STR.h"
#include "../Include/XmlParser.h"


CXmlPathNode::CXmlPathNode(const std::string &strNodeName) : m_strFullName(strNodeName)
{
    do
    {
        // 查找 & 符号
        std::string::size_type nPos = strNodeName.find('&');
        if (nPos == std::string::npos)
        {
            m_strName = strNodeName;
            m_nIndex = 0;
            break;
        }

        m_strName.assign(strNodeName.c_str(), nPos);
        m_nIndex = atoi(strNodeName.c_str() + nPos + 1);
    } while (0);
}


CXmlParser::CXmlParser()
{
}

CXmlParser::~CXmlParser()
{
}

Bool CXmlParser::LoadFile(LPCSTR lpszFileName)
{
    return (m_doc.LoadFile(lpszFileName) == tinyxml2::XML_NO_ERROR);
}

Bool CXmlParser::LoadContent(LPCSTR lpszContent, Int32 nLen)
{
    return (m_doc.Parse(lpszContent, nLen) == tinyxml2::XML_NO_ERROR);
}

Bool CXmlParser::GetContent(LPCSTR lpszPath, std::string &strValue) const
{
    Bool bSuccess = False;

    do
    {
        std::list<CXmlPathNode> lstNodes;
        ExplodePathNode(lpszPath, lstNodes);
        if (lstNodes.empty())
        {
            break;
        }

        std::list<CXmlPathNode>::const_iterator it = lstNodes.begin();
        const tinyxml2::XMLElement *pElement = m_doc.FirstChildElement(it->GetName().c_str());
        pElement = LocateSiblingElement(pElement, *it);

        for (++it; it != lstNodes.end() && pElement != NULL; ++it)
        {
            pElement = pElement->FirstChildElement(it->GetName().c_str());
            if (pElement != NULL)
            {
                pElement = LocateSiblingElement(pElement, *it);
            }
        }

        if (NULL == pElement)
        {
            break;
        }

        strValue.clear();
        LPCSTR lpszText = pElement->GetText();
        if (NULL != lpszText)
        {
            strValue = lpszText;
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

Bool CXmlParser::GetContent(LPCSTR lpszPath, Int32 &nValue) const
{
    std::string strValue;
    if (!GetContent(lpszPath, strValue))
    {
        return False;
    }

    nValue = STR::Str2Int32(strValue.c_str());
    return True;
}

Bool CXmlParser::GetContent(LPCSTR lpszPath, Int64 &lValue) const
{
    std::string strValue;
    if (!GetContent(lpszPath, strValue))
    {
        return False;
    }

    lValue = STR::Str2Int64(strValue.c_str());
    return True;
}

Bool CXmlParser::GetContent(LPCSTR lpszPath, std::list<std::string> &lstValues) const
{
    Bool bSuccess = False;

    do
    {
        std::list<CXmlPathNode> lstNodes;
        ExplodePathNode(lpszPath, lstNodes);
        if (lstNodes.empty())
        {
            break;
        }

        std::list<CXmlPathNode>::const_iterator it = lstNodes.begin();
        const tinyxml2::XMLElement *pElement = m_doc.FirstChildElement(it->GetName().c_str());
        pElement = LocateSiblingElement(pElement, *it);

        for (++it; it != lstNodes.end() && pElement != NULL; ++it)
        {
            pElement = pElement->FirstChildElement(it->GetName().c_str());
            if (pElement != NULL)
            {
                pElement = LocateSiblingElement(pElement, *it);
            }
        }

        if (NULL == pElement)
        {
            break;
        }

        lstValues.clear();
        for (; pElement != NULL; pElement = pElement->NextSiblingElement(pElement->Name()))
        {
            LPCSTR lpszText = pElement->GetText();
            if (lpszText != NULL)
            {
                lstValues.push_back(lpszText);
            }
            else
            {
                lstValues.push_back("");
            }
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

Bool CXmlParser::GetContent(LPCSTR lpszPath, std::vector<std::string> &vecValues) const
{
    Bool bSuccess = False;

    do
    {
        std::list<CXmlPathNode> lstNodes;
        ExplodePathNode(lpszPath, lstNodes);
        if (lstNodes.empty())
        {
            break;
        }

        std::list<CXmlPathNode>::const_iterator it = lstNodes.begin();
        const tinyxml2::XMLElement *pElement = m_doc.FirstChildElement(it->GetName().c_str());
        pElement = LocateSiblingElement(pElement, *it);

        for (++it; it != lstNodes.end() && pElement != NULL; ++it)
        {
            pElement = pElement->FirstChildElement(it->GetName().c_str());
            if (pElement != NULL)
            {
                pElement = LocateSiblingElement(pElement, *it);
            }
        }

        if (NULL == pElement)
        {
            break;
        }

        vecValues.clear();
        for (; pElement != NULL; pElement = pElement->NextSiblingElement(pElement->Name()))
        {
            LPCSTR lpszText = pElement->GetText();
            if (lpszText != NULL)
            {
                vecValues.push_back(lpszText);
            }
            else
            {
                vecValues.push_back("");
            }
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

Bool CXmlParser::GetAttribute(LPCSTR lpszPath, LPCSTR lpszAttribute, std::string &strValue) const
{
    Bool bSuccess = False;

    do
    {
        std::list<CXmlPathNode> lstNodes;
        ExplodePathNode(lpszPath, lstNodes);
        if (lstNodes.empty())
        {
            break;
        }

        std::list<CXmlPathNode>::const_iterator it = lstNodes.begin();
        const tinyxml2::XMLElement *pElement = m_doc.FirstChildElement(it->GetName().c_str());
        pElement = LocateSiblingElement(pElement, *it);

        for (++it; it != lstNodes.end() && pElement != NULL; ++it)
        {
            pElement = pElement->FirstChildElement(it->GetName().c_str());
            if (pElement != NULL)
            {
                pElement = LocateSiblingElement(pElement, *it);
            }
        }

        if (NULL == pElement)
        {
            break;
        }

        LPCSTR lpszText = pElement->Attribute(lpszAttribute);
        if (NULL == lpszText)
        {
            break;
        }

        strValue.clear();
        strValue = lpszText;

        bSuccess = True;
    } while (0);

    return bSuccess;
}

Bool CXmlParser::GetAttribute(LPCSTR lpszPath, LPCSTR lpszAttribute, Int32 &nValue) const
{
    std::string strValue;
    if (!GetAttribute(lpszPath, lpszAttribute, strValue))
    {
        return False;
    }

    nValue = STR::Str2Int32(strValue.c_str());
    return True;
}

Bool CXmlParser::GetAttribute(LPCSTR lpszPath, LPCSTR lpszAttribute, Int64 &lValue) const
{
    std::string strValue;
    if (!GetAttribute(lpszPath, lpszAttribute, strValue))
    {
        return False;
    }

    lValue = STR::Str2Int64(strValue.c_str());
    return True;
}

Int32 CXmlParser::GetChildNum(LPCSTR lpszPath) const
{
    Int32 nCount = 0;

    do
    {
        std::list<CXmlPathNode> lstNodes;
        ExplodePathNode(lpszPath, lstNodes);
        if (lstNodes.empty())
        {
            break;
        }

        std::list<CXmlPathNode>::const_iterator it = lstNodes.begin();
        const tinyxml2::XMLElement *pElement = m_doc.FirstChildElement(it->GetName().c_str());
        pElement = LocateSiblingElement(pElement, *it);

        for (++it; it != lstNodes.end() && pElement != NULL; ++it)
        {
            pElement = pElement->FirstChildElement(it->GetName().c_str());
            if (pElement != NULL)
            {
                pElement = LocateSiblingElement(pElement, *it);
            }
        }

        if (NULL == pElement)
        {
            break;
        }

        pElement = pElement->FirstChildElement();
        while (pElement != NULL)
        {
            ++nCount;
            pElement = pElement->NextSiblingElement();
        }
    } while (0);

    return nCount;
}

/*
 * 功能: 将 XML 路径解析为有序的 CXmlPathNode 节点集合
 */
void CXmlParser::ExplodePathNode(LPCSTR lpszPath, std::list<CXmlPathNode> &lstNodes) const
{
    std::list<std::string> lstItems;
    std::list<std::string>::const_iterator it;

    STR::Explode(lpszPath, "/", lstItems);
    for (it = lstItems.begin(); it != lstItems.end(); ++it)
    {
        CXmlPathNode pathNode(*it);
        lstNodes.push_back(pathNode);
    }
}

const tinyxml2::XMLElement* CXmlParser::LocateSiblingElement(const tinyxml2::XMLElement *pElement, const CXmlPathNode &node) const
{
    Int32 nLoopTimes = node.GetIndex();
    if (nLoopTimes < 0)
    {
        return NULL;
    }

    while (nLoopTimes-- && pElement != NULL)
    {
        pElement = pElement->NextSiblingElement(node.GetName().c_str());
    }

    return pElement;
}
