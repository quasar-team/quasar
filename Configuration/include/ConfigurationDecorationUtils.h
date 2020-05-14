/* © Copyright CERN, 2020. All rights not expressly granted are reserved. */
/*
 * ConfigurationDecorationUtils.h
 *
 *  Created on: May 14, 2020
 *      Author: Ben Farnham
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
 *
 */

#ifndef CONFIGURATION_DECORATION_UTILS_H_
#define CONFIGURATION_DECORATION_UTILS_H_

#include <Configuration.hxx>

namespace Configuration
{
  namespace DecorationUtils
  {

    template< typename TParent, typename TChildren, typename TChild >
    void push_back(TParent& parent, TChildren& children, const TChild& child, const size_t childTypeId)
    {
      children.push_back(child);
      const xml_schema::content_order orderingElement(childTypeId, children.size()-1);
      parent.content_order().push_back(orderingElement);
    };

  } // namespace DecorationUtils
} // namespace Configuration

#endif /* CONFIGURATION_DECORATION_UTILS_H_ */
