| The Generic OPC UA Server Framework
| Design File Manual

Class
=====

| 

Cachevariable
-------------

Attributes of cachevariable
---------------------------

addressSpaceWrite
~~~~~~~~~~~~~~~~~

makeSetGet
~~~~~~~~~~

| This attribute is deprecated and will be completely removed soon.

dataType
~~~~~~~~

| 

initializeWith
~~~~~~~~~~~~~~

| Determines what the cachevariable will be initialized with before any
  custom code or any OPC UA client write is effective.

-  When configuration, an appropriate entry is made in the
   Configuration.xsd file which in turn adds s

| 
| initialValue

initialStatus
~~~~~~~~~~~~~

| When initializeWith=valueAndStatus, this attribute gives the initial
  status of the cachevariable.
| When initializeWith is different, this attribute is ignored.

isKey
~~~~~

nullPolicy
~~~~~~~~~~

-  When nullForbidden, the cachevariable is never allowed to contain
   NULL. This applies to setters (Device Logic can't set NULL), OPC UA
   Client write operations (write operation carrying NULL will be
   denied) and initialization (when initializeFrom=valueAndStatus,
   initialValue is mandatory to be given). Also, thanks to this setting
   a short getter will be created.
-  When nullAllowed, there is no restriction towards NULL.

| 

Relations between nullPolicy, initializeWith and initialValue attributes
------------------------------------------------------------------------

| 

+-----------------+-----------------+-----------------+-----------------+
| nullPolicy      | initializeWith  | initialValue    | Comment         |
+-----------------+-----------------+-----------------+-----------------+
| nullAllowed     | configuration   | NOT RELEVANT    |                 |
+-----------------+-----------------+-----------------+-----------------+
| nullAllowed     | valueAndStatus  | -  If not       |                 |
|                 |                 |    present, the |                 |
|                 |                 |    value is     |                 |
|                 |                 |    initialized  |                 |
|                 |                 |    as NULL.     |                 |
|                 |                 | -  If present,  |                 |
|                 |                 |    the value is |                 |
|                 |                 |    initialized  |                 |
|                 |                 |    with the     |                 |
|                 |                 |    attribute    |                 |
|                 |                 |    contents.    |                 |
+-----------------+-----------------+-----------------+-----------------+
| nullForbidden   | configuration   | NOT RELEVANT    |                 |
+-----------------+-----------------+-----------------+-----------------+
| nullForbidden   | valueAndStatus  | MUST BE PRESENT |                 |
+-----------------+-----------------+-----------------+-----------------+

| 
