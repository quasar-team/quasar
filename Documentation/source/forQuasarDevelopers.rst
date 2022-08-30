Notes for quasar developers...
==============================

| *... and for those who use quasar design transformations for extension
  modules*

| Written by Piotr P. Nikiel, Nov 2019
| Edited by Piotr P. Nikiel in May 2020 (after moving to Jinja-based
  transforms)
| Email contact: quasar-developers@cern.ch

Notes for developers of quasar design transformations (either within quasar or extension modules)
-------------------------------------------------------------------------------------------------

| quasar switched to Jinja-based transforms as of release 1.4.0.
| Some notes might be useful for quasar transforms developers and for
  quasar extension modules (like
  `UaObjects <https://github.com/quasar-team/UaoForQuasar>`__,
  `Cacophony <https://github.com/quasar-team/Cacophony>`__ or
  `Poverty <https://github.com/quasar-team/Poverty>`__).

#. Both in quasar-supplied transforms (i.e. whatever comes with quasar
   itself) as well as with extension modules, you are invited to use the
   **DesignInspector** and **Oracle** interfaces. You should not invent
   any method to open quasar design on your own.

#. If you develop an extension module and want to perform design
   transformation there (note: not every quasar extension module does
   this!)

#. 

   #. you need some Python program in your extension module which will
      call quasar tooling,

   #. you need to attach some Jinja2 transforms that do your thing (keep
      them in templates/ directory under your module),

      -  Your Jinja transform should indent by two (2) spaces on every
         block, either Jinja block or whatever is being generated (e.g.
         C++).
         Example from actual transform generating C++: (note that indent
         level increases both with new Jinja control blocks as well as
         with C++ blocks)

         .. container:: mycode

            {% for sv in this.sourcevariable %}
              {% if sv.get('addressSpaceRead') == 'asynchronous' or
            sv.get('addressSpaceRead') == 'synchronous' %}
                UaStatus D{{className}}::read{{sv.get('name')|capFirst}}
            ()
                {
                  return OpcUa_BadNotImplemented;
                }
              {% endif %}
            {% endfor %}

   #. if you need some in-Python processing which is difficult to do in
      Jinja, then the guildelines are:

   #. 

      #. write a function/method/lamda in your Python program (parent
         point 1 above), e.g.:

         .. container:: mycode

            def quasar_data_type_to_dpt_type_constant(quasar_data_type):
              return "my_type" + quasar_data_type # it's just an example

      #. then we pass your function(s)/method(s)/lambda(s) using the
         additionalParam argument:

         .. container:: mycode

            additional_params = { 'mapper' :
            quasar_data_type_to_dpt_type_constant }
               transformDesign(
                    os.path.join(cacophony_root, 'templates',
            'designToDptCreation.jinja'), # path to Jinja transform
                    outputFile=os.path.join(thisModuleName, 'generated',
            'createDpts.ctl'),
                    requiresMerge=False,
                    astyleRun=True,
                    additionalParam=\ **additional_params**)

      #. in the Jinja transform, you can use it this way:

         .. container:: mycode

            dynAppend(xxdepei, makeDynInt(0, {{(cv.get('dataType'))}}));

      #. If you need some inspiration, look for example here:
         https://github.com/quasar-team/Cacophony/blob/master/generateStuff.py

#. On debugging: if you want to print some information from Jinja
   transforms, do it this way:

   .. container:: mycode

      {{debug("WARNING: type is unsupported:", quasar_data_type)}}

   Consecutive arguments will be separated by a space.

Conventions regarding Python code for quasar tooling
----------------------------------------------------

| The note here applies to people developing quasar scripting, like
  quasar.py and other py scripts in the FrameworkInternals directory.

Indentation
~~~~~~~~~~~

| Developers should follow
  `PEP8 <https://www.python.org/dev/peps/pep-0008/#indentation>`__.

| Note that there is some tooling which can help maintain proper
  indentation, for instance we recommend autopep8.
| The way to invoke it might be:

.. container:: mycode

   find . -name \\*.py -exec autopep8 --select E101,W291,W293 -i {} \\;

| 

Line ending encoding
~~~~~~~~~~~~~~~~~~~~

| You should use LF line endings. You can use dos2unix if in need.

Encoding
~~~~~~~~

| You should use UTF-8.

Shebang
~~~~~~~

| You should use the following shebang:

.. container:: mycode

   #!/usr/bin/env python
