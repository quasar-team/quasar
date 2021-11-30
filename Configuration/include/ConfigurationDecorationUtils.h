/* © Copyright CERN, 2020. All rights not expressly granted are reserved. */
/*
 * ConfigurationDecorationUtils.h
 *
 *  Created on: May 14, 2020
 *      Author: Ben Farnham
 *      Author: Piotr Nikiel
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
    /**
    * push_back is a configuration decoration helper function. This function *MUST*
    * be used when decorating the configuration object tree (i.e. adding new
    * object instances). This function handles both
    * 1. Addition: adding the object in the specified tree location.
    * 2. Content Ordering: maintaining the ordering mechanism quasar uses to process
    *    configuration elements in the correct order.
    *
    * @param parent: Parent tree node.
    * @param children: The collection of children (owned by the parent) to which the
    *        new child will be appended.
    * @param child: The new child object
    * @param childTypeId: The ordering type ID (as gen'd by xsdcxx) of the new child 
    *        object          
    */
    template< typename TParent, typename TChildren, typename TChild >
    void push_back(TParent& parent, TChildren& children, const TChild& child, const size_t childTypeId)
    {
      children.push_back(child);
      const xml_schema::content_order orderingElement(childTypeId, children.size()-1);
      parent.content_order().push_back(orderingElement);
    };

    template< typename TParent, typename TChildren>
    void clear(TParent& parent, TChildren& children, const size_t childTypeId)
    {

      children.clear();

      auto one_past_removed_iter = std::remove_if(
          std::begin(parent.content_order()),
          std::end(parent.content_order()),
          [childTypeId](decltype(parent.content_order().front())& content_order_elem){return content_order_elem.id == childTypeId;});

      parent.content_order().erase(one_past_removed_iter, std::end(parent.content_order()));
    }

  } // namespace DecorationUtils
} // namespace Configuration

#endif /* CONFIGURATION_DECORATION_UTILS_H_ */
