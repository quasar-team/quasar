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

#ifndef __DTestClass__H__
#define __DTestClass__H__

#include <memory>
#include <string>

#include <Configuration.hxx>

#include <Base_DTestClass.h>

namespace Device
{

class
    DTestClass
    : public Base_DTestClass
{

public:
    explicit DTestClass (
        const Configuration::TestClass& config,
        Parent_DTestClass* parent
    ) ;
    ~DTestClass ();

    // Canary for the downstream-user-code contract of the xsd-generated
    // Configuration (OPCUA-3456): pins the parse function's smart-pointer
    // species (unique_ptr, from xsdcxx --std c++11) and the configentry
    // accessors. Never called at runtime; a codegen or toolchain change that
    // breaks user servers breaks this compile first.
    std::unique_ptr<Configuration::Configuration> parseLikeDownstreamUserCode ( const std::string& fileName );

private:
    DTestClass( const DTestClass& other );
    DTestClass& operator=(const DTestClass& other);

    unsigned int m_uint32Entry;
    bool m_booleanEntry;

};

}

#endif // __DTestClass__H__
