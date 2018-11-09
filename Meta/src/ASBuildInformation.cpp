
/*  © Copyright CERN, 2015. All rights not expressly granted are reserved.
    Authors(from Quasar team): Piotr Nikiel

    This file is part of Quasar.

    Quasar is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public Licence as published by
    the Free Software Foundation, either version 3 of the Licence.
    Quasar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public Licence for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Quasar.  If not, see <http://www.gnu.org/licenses/>.

    This file was completely generated by Quasar (additional info: using transform designToClassBody.xslt)
    on 2018-11-09T17:06:17.136+01:00
 */


#include <iostream>
#include <climits>

#include <boost/lexical_cast.hpp>

#include <ArrayTools.h>
#include <Utils.h>

#include <SourceVariables.h>



#include <ASBuildInformation.h>


#include <DBuildInformation.h>


namespace AddressSpace
{



/*ctr*/
ASBuildInformation::ASBuildInformation (
    UaNodeId parentNodeId,
    const UaNodeId& typeNodeId,
    ASNodeManager *nm,
	const std::string& host, const std::string& timestamp, const std::string commitID, const std::string& toolkitLibs):
    OpcUa::BaseObjectType (
        /*nodeId*/
        nm->makeChildNodeId(parentNodeId,"ASBuildInformation"), "ASBuildInformation", nm->getNameSpaceIndex(), nm),
    m_typeNodeId (typeNodeId)


    ,


    m_BuildHost (new

                 OpcUa::BaseDataVariableType


                 (nm->makeChildNodeId(
                      this->nodeId(),
                      UaString("BuildHost")), UaString("BuildHost"), nm->getNameSpaceIndex(), UaVariant(UaString(host.c_str())),

                  OpcUa_AccessLevels_CurrentRead
                  , nm))
    ,


    m_BuildTimestamp (new

                      OpcUa::BaseDataVariableType


                      (nm->makeChildNodeId(
                           this->nodeId(),
                           UaString("BuildTimestamp")), UaString("BuildTimestamp"), nm->getNameSpaceIndex(), UaVariant(UaString(timestamp.c_str())),

                       OpcUa_AccessLevels_CurrentRead
                       , nm))
    ,


    m_CommitID (new

                OpcUa::BaseDataVariableType


                (nm->makeChildNodeId(
                     this->nodeId(),
                     UaString("CommitID")), UaString("CommitID"), nm->getNameSpaceIndex(), UaVariant(UaString(commitID.c_str())),

                 OpcUa_AccessLevels_CurrentRead
                 , nm))
    ,


    m_ToolkitLibs (new

                   OpcUa::BaseDataVariableType


                   (nm->makeChildNodeId(
                        this->nodeId(),
                        UaString("ToolkitLibs")), UaString("ToolkitLibs"), nm->getNameSpaceIndex(), UaVariant(UaString(toolkitLibs.c_str())),

                    OpcUa_AccessLevels_CurrentRead
                    , nm))
    ,
    m_deviceLink (0)



{

    UaStatus s;
    UaVariant v;


    s = nm->addNodeAndReference( parentNodeId, this, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        LOG(Log::ERR) << "While addNodeAndReference from " << parentNodeId.toString().toUtf8() << " to " << this->nodeId().toString().toUtf8() << " : ";
        ASSERT_GOOD(s);
    }



    m_BuildHost->setDataType(UaNodeId( OpcUaType_String, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0

    // found scalar signature: can simply load the variant with the scalar
    // fnc:dataTypeToVariantSetter(@dataType)
    v.setString (
        host.c_str()
    );
    m_BuildHost->setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);


    s = nm->addNodeAndReference(
            this,
            m_BuildHost, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_BuildHost->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }



    m_BuildTimestamp->setDataType(UaNodeId( OpcUaType_String, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0

    // found scalar signature: can simply load the variant with the scalar
    // fnc:dataTypeToVariantSetter(@dataType)
    v.setString (
        timestamp.c_str()
    );
    m_BuildTimestamp->setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);


    s = nm->addNodeAndReference(
            this,
            m_BuildTimestamp, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_BuildTimestamp->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }



    m_CommitID->setDataType(UaNodeId( OpcUaType_String, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0

    // found scalar signature: can simply load the variant with the scalar
    // fnc:dataTypeToVariantSetter(@dataType)
    v.setString (
        commitID.c_str()
    );
    m_CommitID->setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);


    s = nm->addNodeAndReference(
            this,
            m_CommitID, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_CommitID->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }



    m_ToolkitLibs->setDataType(UaNodeId( OpcUaType_String, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0

    // found scalar signature: can simply load the variant with the scalar
    // fnc:dataTypeToVariantSetter(@dataType)
    v.setString (
        toolkitLibs.c_str()
    );
    m_ToolkitLibs->setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);


    s = nm->addNodeAndReference(
            this,
            m_ToolkitLibs, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_ToolkitLibs->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }




}





ASBuildInformation::~ASBuildInformation ()
{

    if (m_deviceLink != 0)
    {
        LOG(Log::ERR) << "deviceLink not zero!!";
    }

}




/* generate setters and getters (if requested) */


UaStatus ASBuildInformation::setBuildHost (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{

    UaVariant v;

    v.setString( value );

    return m_BuildHost->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}


UaStatus ASBuildInformation::getBuildHost (UaString & r) const
{
    UaVariant v (* (m_BuildHost->value(/*session*/0).value()));

    if (v.type() == OpcUaType_String)
    {
        r = v.toString();
        return OpcUa_Good;
    }
    else
        return OpcUa_Bad;

}

/* short getter (possible because this variable will never be null) */
UaString ASBuildInformation::getBuildHost () const
{
    UaVariant v (* m_BuildHost->value (0).value() );
    UaString v_value;
    v_value = v.toString();
    return v_value;
}


UaStatus ASBuildInformation::setBuildTimestamp (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{

    UaVariant v;

    v.setString( value );

    return m_BuildTimestamp->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}


UaStatus ASBuildInformation::getBuildTimestamp (UaString & r) const
{
    UaVariant v (* (m_BuildTimestamp->value(/*session*/0).value()));

    if (v.type() == OpcUaType_String)
    {
        r = v.toString();
        return OpcUa_Good;
    }
    else
        return OpcUa_Bad;

}

/* short getter (possible because this variable will never be null) */
UaString ASBuildInformation::getBuildTimestamp () const
{
    UaVariant v (* m_BuildTimestamp->value (0).value() );
    UaString v_value;
    v_value = v.toString();
    return v_value;
}


UaStatus ASBuildInformation::setCommitID (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{

    UaVariant v;

    v.setString( value );

    return m_CommitID->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}


UaStatus ASBuildInformation::getCommitID (UaString & r) const
{
    UaVariant v (* (m_CommitID->value(/*session*/0).value()));

    if (v.type() == OpcUaType_String)
    {
        r = v.toString();
        return OpcUa_Good;
    }
    else
        return OpcUa_Bad;

}

/* short getter (possible because this variable will never be null) */
UaString ASBuildInformation::getCommitID () const
{
    UaVariant v (* m_CommitID->value (0).value() );
    UaString v_value;
    v_value = v.toString();
    return v_value;
}


UaStatus ASBuildInformation::setToolkitLibs (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{

    UaVariant v;

    v.setString( value );

    return m_ToolkitLibs->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}


UaStatus ASBuildInformation::getToolkitLibs (UaString & r) const
{
    UaVariant v (* (m_ToolkitLibs->value(/*session*/0).value()));

    if (v.type() == OpcUaType_String)
    {
        r = v.toString();
        return OpcUa_Good;
    }
    else
        return OpcUa_Bad;

}

/* short getter (possible because this variable will never be null) */
UaString ASBuildInformation::getToolkitLibs () const
{
    UaVariant v (* m_ToolkitLibs->value (0).value() );
    UaString v_value;
    v_value = v.toString();
    return v_value;
}





/* generate delegates (if requested) */





/* generate device logic link */


void ASBuildInformation::linkDevice( Device::DBuildInformation *deviceLink)
{
    if (m_deviceLink != 0)
    {
        /* This is an error -- device can be linked at most in the object's lifetime. */
        //TODO After error handling is discussed, abort in smarter way
        abort();

    }
    else
        m_deviceLink = deviceLink;
}


void ASBuildInformation::unlinkDevice ()
{
    m_deviceLink = 0;
}


}

