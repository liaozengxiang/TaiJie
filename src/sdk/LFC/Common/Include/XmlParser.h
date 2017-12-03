/*
 * XmlParser.h
 *
 *  Created on: Jun 01, 2016
 *      Author: lory
 *
 * Usage: 
 *     <name_list>
 *         <name age="28">Jimmy</name>
 *         <name age="29">Tony</name>
 *         <name age="30">Lory</name>
 *         <other>other value</other>
 *     </name_list>
 *
 *     // 获得子节点数量
 *     GetChildNum("name_list") => 3
 *     GetChildNum("name_list/name&0") => 0
 *
 *     // 获得指定节点的内容
 *     GetContent("name_list/name&0", strName) => "Jimmy"
 *     GetContent("name_list/name&1", strName) => "Tony"
 *     GetContent("name_list/name") => array("Jimmy", "Tony", "Lory");
 *
 *     // 获得指定节点的指定属性
 *     GetAttribute("name_list/name&2", "age", nAge) => 30
 *     GetAttribute("name_list/name&2", "age", strAge) => "30"
 */

#ifndef ___XML__PARSER__20160601___
#define ___XML__PARSER__20160601___

#include <string>
#include <list>
#include <vector>
#include "OSType.h"
#include "Tinyxml2.h"

// 描述 XML 节点路径的一个子节点
class CXmlPathNode
{
public:
    CXmlPathNode(const std::string &strNodeName);

public:
    const std::string& GetName() const { return m_strName; }
    const std::string& GetFullName() const { return m_strFullName; }
    Int32 GetIndex() const { return m_nIndex; }

protected:
    std::string                     m_strFullName;    // XML 节点原始名称，如: name&0/name/name&1
    std::string                     m_strName;        // XML 节点节点名称
    Int32                           m_nIndex;         // XML 节点的序号，若同一级存在相同的名称，则用 &0/&1/&2 进行标识
};

class CXmlParser
{
public:
    CXmlParser();
    ~CXmlParser();

public:
    Bool LoadFile(LPCSTR lpszFileName);
    Bool LoadContent(LPCSTR lpszContent, Int32 nLen = -1);

    Bool GetContent(LPCSTR lpszPath, std::string &strValue) const;
    Bool GetContent(LPCSTR lpszPath, Int32 &nValue) const;
    Bool GetContent(LPCSTR lpszPath, Int64 &lValue) const;
    Bool GetContent(LPCSTR lpszPath, std::list<std::string> &lstValues) const;
    Bool GetContent(LPCSTR lpszPath, std::vector<std::string> &vecValues) const;
    Bool GetAttribute(LPCSTR lpszPath, LPCSTR lpszAttribute, std::string &strValue) const;
    Bool GetAttribute(LPCSTR lpszPath, LPCSTR lpszAttribute, Int32 &nValue) const;
    Bool GetAttribute(LPCSTR lpszPath, LPCSTR lpszAttribute, Int64 &lValue) const;

    Int32 GetChildNum(LPCSTR lpszPath) const;

protected:
    void ExplodePathNode(LPCSTR lpszPath, std::list<CXmlPathNode> &lstNodes) const;
    const tinyxml2::XMLElement* LocateSiblingElement(const tinyxml2::XMLElement *pElement, const CXmlPathNode &node) const;

protected:
    tinyxml2::XMLDocument           m_doc;
};

#endif
