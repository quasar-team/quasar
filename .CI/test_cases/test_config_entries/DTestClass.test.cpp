/*  © Copyright CERN, 2026. All rights not expressly granted are reserved.
    @author Paris Moschovakos

    Quasar is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public Licence as published by
    the Free Software Foundation, either version 3 of the Licence.
    Quasar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public Licence for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Configuration.hxx>

#include <DTestClass.h>
#include <ASTestClass.h>

namespace Device
{

DTestClass::DTestClass (
    const Configuration::TestClass & config,
    Parent_DTestClass * parent
):
    Base_DTestClass( config, parent),
    m_uint32Entry( config.OpcUaUInt32_scalar() ),
    m_booleanEntry( config.OpcUaBoolean_scalar() )
{
}

DTestClass::~DTestClass ()
{
}

std::unique_ptr<Configuration::Configuration> DTestClass::parseLikeDownstreamUserCode ( const std::string& fileName )
{
    std::unique_ptr<Configuration::Configuration> configuration =
        Configuration::configuration( fileName, ::xml_schema::flags::keep_dom );
    return configuration;
}

}
