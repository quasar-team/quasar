/* © Copyright CERN, 2022.  All rights not expressly granted are reserved.
 * metaBackwardsCompatibility.h
 *
 *  Created on: Jul 19, 2022
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <Configuration.hxx>

// Forms part of the functionality block marked with _META_BACKWARDS_COMPATIBILITY_HACK_
// handles conversion from old-skool configurations to new-skool meta to maintain
// backwards compatilbility for old-skool meta configuration XML in new-skool meta.
// 
// Old-skool meta: had a dedicated XSD shema injected into Configuration.xsd)
//   Configurations looked like
//   <StandardMetaData>
//    <Log>
//      <GeneralLogLevel logLevel="INF"/>
//      <ComponentLogLevels>
//        <ComponentLogLevel componentName="A" logLevel="INF"/>
//        <ComponentLogLevel componentName="B" logLevel="DBG"/>
//        <ComponentLogLevel componentName="C" logLevel="TRC"/>
//      </ComponentLogLevels>
//    </Log>
//  </StandardMetaData>
//
// New-skool meta: meta XSD generated from quasar templates by injecting meta-design.xml into Design.xml
// <StandardMetaData>
//   <Log>
//     <LogLevel name="GeneralLogLevel" logLevel="DBG"/>
//     <ComponentLogLevels>
//       <LogLevel name="A" logLevel="INF"/>
//       <LogLevel name="B" logLevel="DBG"/>
//       <LogLevel name="C" logLevel="TRC"/>
//     </ComponentLogLevels>
//   </Log>
//   <SourceVariableThreadPool maxThreads="99" minThreads="0"/>
// </StandardMetaData>
//
namespace Meta
{
    namespace BackwardsCompatibilityUtils
    {
        void convertOldGeneralLogLevel(Configuration::Log& parent);
        void convertOldComponentLogLevel(Configuration::ComponentLogLevels& parent);
    } // namespace BackwardsCompatibilityUtils
} // namespace Meta