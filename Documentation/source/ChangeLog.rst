ChangeLog
=========

.. raw:: html

	<table style="text-align: left; width: 100%;" cellspacing="1"
	      cellpadding="4" border="0" class="table-changelog">
	      <tbody>
	        <tr style="background-color: rgb(34, 84, 152); color: rgb(255, 255, 255);">
	          <td style="vertical-align: top; color: rgb(255, 255, 255);"><strong>Version</strong><br></td>
	          <td style="vertical-align: top; color: rgb(255, 255, 255);"><strong>Changes introduced</strong><br></td>
	          <td style="vertical-align: top; color: rgb(255, 255, 255);"><strong>Possible backward incompatibilities</strong><br></td>
	          <td style="vertical-align: top; color: rgb(255, 255, 255);"><strong>JIRA Release notes</strong><br></td>
	          <td valign="top"><strong>Testing outcome</strong><br></td>
	        </tr>

            <tr>
                <!-- Version -->
                <td valign="top">1.6.1<font size="-1"><br>(10-Aug-2023)</font><br></td>
                <!-- Changes introduced -->
                <td valign="top"><br>This release focuses on enhancing the functionality of calculated variables. It also addresses issues related to the uasdk travis test and introduces necessary dependencies in quasar's uasdk dockerfile. Additionally, the documentation has been updated to provide clarity on the manual and auto-update control for calculated variables.</td>
                <!-- Possible backward incompatibilities -->
                <td valign="top"><br>(none known)</td>
                <!-- JIRA Release notes -->
                <td valign="top">
                    Task
                    <ul>
                        <li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3040'>OPCUA-3040</a>] - Calculated variables per request for CanOpenNG</li>
                        <li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3072'>OPCUA-3072</a>] - Calculated variables uasdk travis test uses wrong backend</li>
                        <li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3075'>OPCUA-3075</a>] - quasar's uasdk dockerfile misses dependency</li>
                    </ul>
                    User Documentation
                    <ul>
                        <li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3077'>OPCUA-3077</a>] - Calculated Variables Documentation for Manual Update and Auto Update Control</li>
                    </ul>
                </td>
                <td valign="top">quasar test suite notes:<br>
                    <ol>
                        <li>open62541_test_methods not passing for the method arguments behaving differently from UASDK. This will be <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further investigated</a>.</li>
                        <li>StandardMetaData is ignored from testing in all tests due to some existing noncompliance.</li>
                    </ol>
                </td>
            </tr>

            <tr>
                <td valign="top">1.6.0<font size="-1"><br>(10-Jul-2023)</font><br></td>
                <td valign="top"><br>This release introduces the quasar Sanitizers, a new feature that assists developers in identifying and fixing code issues, such as bugs and memory leaks, during compilation or execution. This tool can be easily toggled on and off, depending on your needs. Please refer to the accompanying documentation for detailed instructions on its usage.</td>
                <td valign="top"><br>(none known)</td>
                <td valign="top">
                    Story
                    <ul><li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3008'>OPCUA-3008</a>] - Establish tooling for safer OPC UA servers for quasar users</li></ul>
                    Task
                    <ul><li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3041'>OPCUA-3041</a>] - quasar upgrade_project fails to create missing cmake dir</li></ul>
                    Bug
                    <ul><li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2964'>OPCUA-2964</a>] - Exception when using honkytonk quasar command</li></ul>
                    User Documentation
                    <ul><li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3031'>OPCUA-3031</a>] - Documentation for quasar Sanitizers</li></ul>
                </td>
                <td valign="top">quasar test suite notes:<br>
                    <ol>
                        <li>open62541_test_methods not passing for the method arguments behaving differently from UASDK. This will be <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further investigated</a>.</li>
                        <li>StandardMetaData is ignored from testing in all tests due to some existing noncompliance.</li>
                    </ol>
                </td>
            </tr>

			<tr>

			<td valign="top">1.5.18<font size="-1"><br>
				(01-Mar-2023)</font><br>
			</td>
			<td valign="top">
			<br>
				This release features a much improved Quasar ThreadPool (called internally a supervised threadpool)
				which affects all async (async methods and async source variables) operations of quasar.
				The order of execution of queued async operations within the same synchro domain is now guaranteed to be
				preserved. Also, lower overall latency is expected.
			</td>
			<td valign="top"><br>
				<ul>
					<li>quasar device logic locks are now std::mutex from boost::mutex (a move that was announced already in March 2020)</li>
					<li>to profit from guaranteed order of execution, in case of <emph>not using</emph> the standard options like <code>of_containing_object</code>, etc.,
						the new option <code>handpicked</code> must be chosen and your mutex of choice needs to be registered using <code>configure...</code> methods
						of particular address-space object. Failing to do so will remain to work, but no benefits from order preservation will be observed.
						</li>
						<li>
						when using methods, it is necessary to specify <code>addressSpaceCallUseMutex</code>, note the previous default was "no".
						</li>
				</ul>
			</td>
			<td valign="top">

			        New Feature

			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3000'>OPCUA-3000</a>] -         Metrics for the threadpool: current queued number of jobs, total taken num jobs, total finished num of jobs
			</li>
			</ul>

			        Task

			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2891'>OPCUA-2891</a>] -         Supervised Threadpool, context SourceVars
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2977'>OPCUA-2977</a>] -         QuasarThreadPool.[cpp,h] to be treated as framework files by the installa
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2978'>OPCUA-2978</a>] -         Changelog/notes regarding threadpool changes
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2990'>OPCUA-2990</a>] -         Supervised threadpool, the methods part
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2991'>OPCUA-2991</a>] -         add DesignValidator protection: method call use mutex implies class has mutex
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2992'>OPCUA-2992</a>] -         DesignValidator: deny any synchro domain for synchronous sourcevariable
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2993'>OPCUA-2993</a>] -         Clean-up lock/unlock base mthds
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2994'>OPCUA-2994</a>] -         Optimize out mutices map?
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2996'>OPCUA-2996</a>] -         Follow up of supervised threadpool
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3007'>OPCUA-3007</a>] -         Add synchro domains variety for the async methods test
			</li>
			</ul>

			        Improvement

			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-3005'>OPCUA-3005</a>] -         Make addressSpaceCallUseMutex obligatory
			</li>
			</ul>

			</td>
			<td valign="top">quasar test suite notes:<br>

			<ol>
			<li>open62541_test_methods not passing for the method
				arguments behaving differently from UASDK. This will be
				<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
				investigated</a>.</li>
			<li> StandardMetaData is ignored from testing in all tests
				due to some existing noncompliance.</li>
			</ol>

			</td>
			</tr>

			<tr>

			<td valign="top">1.5.17<font size="-1"><br>
				(16-Feb-2023)</font><br>
			</td>
			<td valign="top">Point release with different incremental updates<br>
			</td>
			<td valign="top"><br>(none known)
			</td>
			<td valign="top">
					Bug<br/>
			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2918'>OPCUA-2918</a>] -         quasar docs: versions description not updated
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2920'>OPCUA-2920</a>] -         Deploying docs fails to create new tag dir
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2959'>OPCUA-2959</a>] -         Error with missing file at fresh project creation
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2974'>OPCUA-2974</a>] -         quasar v.1.5.17-rc0 &quot;./quasar.py build&quot; fails in the arg parsing
			</li>
			</ul>
					Task<br/>
			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2919'>OPCUA-2919</a>] -         Consolidation of duplicates in Docs dir
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2952'>OPCUA-2952</a>] -         quasar&#39;s python_reqs.txt for Python3.9: lxml version is too strict
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2956'>OPCUA-2956</a>] -         Cosmetics: source variable exception missing spaces
			</li>
			</ul>
					Improvement <br/>
			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2962'>OPCUA-2962</a>] -          Rethink quasar CLI
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2980'>OPCUA-2980</a>] -         release_nebula.py to handle the documentation versions
			</li>
			</ul>
			</td>
			<td valign="top">quasar test suite notes:<br>
				<ol>
				<li>open62541_test_methods not passing for the method
					arguments behaving differently from UASDK. This will be
					<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
					investigated</a>.</li>
				<li> StandardMetaData is ignored from testing in all tests
					due to some existing noncompliance.</li>
				</ol>
			</td>
			</tr>

			<tr>

			<td valign="top">1.5.16<font size="-1"><br>
				(02-Dec-2022)</font><br>
			</td>
			<td valign="top">Point release with different incremental updates<br>
			</td>
			<td valign="top"><br>(none known)
			</td>
			<td valign="top">
					Task<br/>
			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2844'>OPCUA-2844</a>] -         Clean up deprecated html documentation files
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2851'>OPCUA-2851</a>] -         what():  Attempted Source Variable operation, but Source Variable threads are not up.
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2870'>OPCUA-2870</a>] -         FreeVariable R/W controls in the config
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2898'>OPCUA-2898</a>] -         Central exception catch: print type
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2916'>OPCUA-2916</a>] -         quasar: Identify/mark potentially unused function
			</li>
			</ul>
					Improvement <br/>
			<ul>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2885'>OPCUA-2885</a>] -         Remove reference to quasar.html when installing the framework
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2886'>OPCUA-2886</a>] -         External files restructured and paths update
			</li>
			<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2888'>OPCUA-2888</a>] -         Consolidate original files
			</li>
			</ul>
			</td>
			<td valign="top">quasar test suite notes:<br>
				<ol>
				<li>open62541_test_methods not passing for the method
					arguments behaving differently from UASDK. This will be
					<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
					investigated</a>.</li>
				<li> StandardMetaData is ignored from testing in all tests
					due to some existing noncompliance.</li>
				</ol>
			</td>
			</tr>
			<tr>
				<td valign="top">1.5.15<font size="-1"><br>
					(17-Nov-2022)</font><br>
				</td>
				<td valign="top">Back-fixes for OPCUA-2767 and OPCUA-2779
					which turned out to be broken.<br>
				</td>
				<td valign="top">Your configuration schema is now stronger
					regarding isKey usage; for duplicate keys previously you'd
					only learn at runtime, now you also learn by the schema!<br>
				</td>
				<td valign="top">Bug
					<br>
					<ul>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2135'>OPCUA-2135</a>]
						- Configurator broken for embedded objects of design-instantiation
					</li>
					</ul>
					<br>
					New Feature
					<br>
					<ul>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2829'>OPCUA-2829</a>]
						- RTD web server in quasar
					</li>
					</ul>
					<br>
					Task
					<br>
					<ul>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2827'>OPCUA-2827</a>]
						- quasar docs review: &quot;main&quot;
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2828'>OPCUA-2828</a>]
						- quasar docs review: alternative backends
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2830'>OPCUA-2830</a>]
						- Update Changelog manually for the last time from html to rst
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2854'>OPCUA-2854</a>]
						- config doc: notification that given element can&#39;t be present more than once, and the notification of defaulted instance name
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2863'>OPCUA-2863</a>]
						- Fix quasar notes and external files paths
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2864'>OPCUA-2864</a>]
						- Pointer to quasar.docs in quasar github main page
					</li>
					</ul>
					<br>
					Improvement
					<br>
					<ul>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2833'>OPCUA-2833</a>]
						- Move RTD Github actions to quasar service account and paths
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2835'>OPCUA-2835</a>]
						- Information missing about EPEL dependencies
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2856'>OPCUA-2856</a>]
						- Pull in quasar LogIt with updated fixes
					</li>
					<li>[<a href='https://its.cern.ch/jira/browse/OPCUA-2860'>OPCUA-2860</a>]
						- Create quasar service account secret and fix references
					</li>
					</ul>
				</td>
				<td valign="top">quasar test suite notes:<br>
					<ol>
					<li>open62541_test_methods not passing for the method
						arguments behaving differently from UASDK. This will be
						<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
						investigated</a>.</li>
					<li> StandardMetaData is ignored from testing in all tests
						due to some existing noncompliance.</li>
					</ol>
				</td>
			</tr>
			<tr>
				<td valign="top">1.5.14<font size="-1"><br>
					(03-Aug-2022)</font><br>
				</td>
				<td valign="top">Back-fixes for OPCUA-2767 and OPCUA-2779
					which turned out to be broken.<br>
				</td>
				<td valign="top">Your configuration schema is now stronger
					regarding isKey usage; for duplicate keys previously you'd
					only learn at runtime, now you also learn by the schema!<br>
				</td>
				<td valign="top">Bug
					<br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2787">OPCUA-2787</a>]
						- defaultInstanceName misbehaving
					</li>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2789">OPCUA-2789</a>]
						- quasar creates non-compiling code in windows
					</li>
					</ul>
					<br>
					New Feature
					<br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2768">OPCUA-2768</a>]
						- Propagate isKey into unique constraint of
						Configuration XSD
					</li>
					</ul>
					<br>
					Task
					<br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2788">OPCUA-2788</a>]
						- A test for defaultInstanceName
					</li>
					</ul>
				</td>
				<td valign="top">quasar test suite notes:<br>
					<ol>
					<li>open62541_test_methods not passing for the method
						arguments behaving differently from UASDK. This will be
						<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
						investigated</a>.</li>
					<li> StandardMetaData is ignored from testing in all tests
						due to some existing noncompliance.</li>
					</ol>
				</td>
			</tr>
			<tr>
				<td valign="top">1.5.13<br>
					<font size="-1">(26-Jul-2022)</font><br>
				</td>
				<td valign="top">Ben's contrib w/ defaulting name of quasar
					objects in the configuration for classes which are always
					singletons (feature driver by StandardMetaData rework), with
					the necessary follow-up.<br>
				</td>
				<td valign="top">Expected impact on certain satellite modules
					(e.g. Cacophony/fwQuasar) <i>if the new feature is used</i>.
					Update relevant satellite modules.<br>
				</td>
				<td valign="top"> New Feature <br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2767">OPCUA-2767</a>]
						- support default name for singletons by configuration
						(i.e. no 'name' attr required in config - default to
						class name) </li>
					</ul>
					<br>
					Task <br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2779">OPCUA-2779</a>]
						- Follow-up of defaulting "name" attribute to be
						explicitly chosen </li>
					</ul>
				</td>
				<td valign="top">quasar test suite notes:<br>
					<ol>
					<li>open62541_test_methods not passing for the method
						arguments behaving differently from UASDK. This will be
						<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
						investigated</a>.</li>
					<li> StandardMetaData is ignored from testing in all tests
						due to some existing noncompliance.</li>
					</ol>
				</td>
			</tr>
			<tr>
				<td valign="top">1.5.12<br>
					<font size="-1">(18-Jul-2022)</font><br>
				</td>
				<td valign="top">New features and progress in clean-up of the
					past artifacts.<br>
				</td>
				<td valign="top">(none known)<br>
				</td>
				<td valign="top">Bug <br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2751">OPCUA-2751</a>]
						- generate as_doc fails for empty d:documentation
						content </li>
					</ul>
					<p> New Feature </p>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2757">OPCUA-2757</a>]
						- Information on UA SDK version in quasar servers </li>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2758">OPCUA-2758</a>]
						- OPC UA toolkit path in server "extra info" </li>
					</ul>
					<br>
					Task <br>
					<ul>
					<li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2750">OPCUA-2750</a>]
						- QuasarServerCallback removal (stale code clean-up
						action) </li>
					</ul>
				</td>
				<td valign="top">quasar test suite notes:<br>
					<ol>
					<li>open62541_test_methods not passing for the method
						arguments behaving differently from UASDK. This will be
						<a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
						investigated</a>.</li>
					<li> StandardMetaData is ignored from testing in all tests
						due to some existing noncompliance.</li>
					</ol>
				</td>
			</tr>
	        <tr>
	          <td valign="top">1.5.11<font size="-1"><br>
	              (25-May-2022)</font><br>
	          </td>
	          <td valign="top">Merge-back from the CANopen (OPCUA-2739) is
	            the main motivation.<br>
	          </td>
	          <td valign="top">(none known) </td>
	          <td valign="top">New Feature
	            <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2739">OPCUA-2739</a>]
	                - Permit CalculatedVariable registration on quasar
	                UaVariant dataType (merge-back from CANopen)
	              </li>
	            </ul>
	            <br>
	            Task
	            <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2702">OPCUA-2702</a>]
	                - Get rid of quasar "device file is older than design"
	                warning
	              </li>
	            </ul>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.</li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.10<br>
	            <font size="-1">(21-Mar-2022)</font><br>
	          </td>
	          <td valign="top">New features and gradual improvements over
	            existing ones.<br>
	            <br>
	            <b>Note</b> the feature OPCUA-2619
	            "defaultConfigInitializerValue for CacheVariable" is not yet
	            considered stable, its API might change. </td>
	          <td valign="top">(none known) </td>
	          <td valign="top">New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2606">OPCUA-2606</a>]
	                - d:documentation for enumerationValue </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2619">OPCUA-2619</a>]
	                - defaultConfigInitializerValue for CacheVariables </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2627">OPCUA-2627</a>]
	                - Add CalculatedVariableGenericFormula in the config_doc
	              </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2622">OPCUA-2622</a>]
	                - Investigate [uastack] Opendir failed !!! Error </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2638">OPCUA-2638</a>]
	                - Switch open62541-compat version we test against in
	                quasar tests </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2666">OPCUA-2666</a>]
	                - Design.xsd indentation fix </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2646">OPCUA-2646</a>]
	                - Default StackTraceSettings to be SYSTEM </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2676">OPCUA-2676</a>]
	                - Main-wide catch of except in case of sth thrown from
	                the QuasarServer ctr </li>
	            </ul>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further




















	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.</li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.9<br>
	            <font size="-1">(17-Feb-2022)</font><br>
	          </td>
	          <td valign="top">Service releases bringing gradual
	            improvements.</td>
	          <td valign="top">(none known)</td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2574">OPCUA-2574</a>]
	                - quasar build ignores non-existing build config file </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2567">OPCUA-2567</a>]
	                - SourceVariables write handler could profit from what()
	                method for an automatic catch of exception </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2522">OPCUA-2522</a>]
	                - Remove
	                http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15
	                (deprecated) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2569">OPCUA-2569</a>]
	                - Improve getFullName() for single item nodes </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2572">OPCUA-2572</a>]
	                - Profit from std::exception::what() in readValue, and
	                add variable addresses </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2578">OPCUA-2578</a>]
	                - quasar manual for CC7: update to python3, remove jre
	                and subversion </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2591">OPCUA-2591</a>]
	                - Put SingleNodeVariable in the quasar design diagrams </li>
	            </ul>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.</li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.8<br>
	            <font size="-1">(09-Dec-2021)</font><br>
	          </td>
	          <td valign="top">Service releases bringing gradual
	            improvements.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2536">OPCUA-2536</a>]
	                - Formula elaboration is forgotten for status formulas </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2544">OPCUA-2544</a>]
	                - quasar 1.5.7 compilation error with gcc 11.1.0 </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2533">OPCUA-2533</a>]
	                - "pow" in CalculatedVariables, as the alternative to
	                "^" operator </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2537">OPCUA-2537</a>]
	                - Content-order-aware sequence removal in configuration
	                decorators </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2538">OPCUA-2538</a>]
	                - Have Configuration.xsd automatically indented/
	                pretty-printed </li>
	            </ul>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.</li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.7<br>
	            <font size="-1">(02-Nov-2021)</font><br>
	          </td>
	          <td valign="top">The main feature is the output from UASDK
	            tracing into stdout, which very much simplifies diagnosing
	            issues related to network, files, permissions and other
	            OS-alike aspects when starting up and running the server.<br>
	            <br>
	            We encourage UASDK-based server developers to turn on
	            UaStackTraceEnabled in the shipped ServerConfig.xml and
	            files and set UaStackTraceLevel to SYSTEM.<br>
	          </td>
	          <td valign="top">CMP00065 is now turned on (i.e. "NEW"
	            behaviour). See also release notes from quasar nebula.b1
	            (this table, some rows below). The proposed way of dealing
	            with it is adding the following entry in your
	            CMakeEpilogue.cmake (and adding this file, in case you do
	            not have it):<br>
	            <br>
	            <pre class="code-java"><meta http-equiv="content-type" content="text/html; charset=windows-1252"><pre class="code-java">set_property(TARGET ${EXECUTABLE} PROPERTY ENABLE_EXPORTS TRUE) </pre></pre>
	            <br>
	          </td>
	          <td valign="top"> New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2492">OPCUA-2492</a>]
	                - UaTrace output to the std out </li>
	            </ul>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2463">OPCUA-2463</a>]
	                - Port CMP0065 policy to new </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2510">OPCUA-2510</a>]
	                - Document CMP00065-related changes in quasar ChangeLog
	              </li>
	            </ul>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.</li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.6<br>
	            <font size="-1">(03-Sep-2021)</font><br>
	          </td>
	          <td valign="top">A release dedicated to CalculatedVariables
	            enhancements and fixes<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top"> Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2466">OPCUA-2466</a>]
	                - No initial computation for any formulas with
	                cache_variables as inputs </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2456">OPCUA-2456</a>]
	                - Escape dash and slash in CalculatedVariables formulas
	                and variable names </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2461">OPCUA-2461</a>]
	                - (Initial) test for Calculated Variables </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2464">OPCUA-2464</a>]
	                - config-entries to instantiate ParserVariables </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2465">OPCUA-2465</a>]
	                - Fix test_cache_variables </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2462">OPCUA-2462</a>]
	                - Update/extend calc vars doc (esp. generic formulas) </li>
	            </ul>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.</li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.5<br>
	            <font size="-1">(27-Jul-2021)</font><br>
	          </td>
	          <td valign="top">Optimized the CI testing, reworked some
	            scripts, cleaned-up. quasar fixed some noncompliance wrt
	            address-space info model. <br>
	          </td>
	          <td valign="top">If you use open62541-compat, please use 1.4.0
	            or later.<br>
	          </td>
	          <td valign="top">
	            <main role="main" id="main" class="aui-page-panel-content">
	              <meta http-equiv="content-type" content="text/html;
	                charset=windows-1252">
	              Bug<br>
	              <ul>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2365">OPCUA-2365</a>]
	                  - quasar CI test "test_cache_variables" non passing as
	                  non-OPCUA compliant</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2418">OPCUA-2418</a>]
	                  - test_config_entry missing DataType (i.e. defaults to
	                  BaseDataType)</li>
	              </ul>
	              <br>
	              Task<br>
	              <ul>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2355">OPCUA-2355</a>]
	                  - quasar servers, when used as shared objects, not to
	                  cannibalize Ctrl-C handler</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2400">OPCUA-2400</a>]
	                  - "nullAllowed" arrays (config-initialized) should
	                  have DataType of BaseDataType</li>
	              </ul>
	              <br>
	              Improvement<br>
	              <ul>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2372">OPCUA-2372</a>]
	                  - quasar to use open2541 1.2X (i.e. open62541-compat
	                  1.4.x) for CI</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2373">OPCUA-2373</a>]
	                  - travis.yml global variable for open62541-compat
	                  version to test against</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2374">OPCUA-2374</a>]
	                  - augment reference NS2 w/ additions from UaSak 0.6</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2421">OPCUA-2421</a>]
	                  - shallow clone of quasar in quasar CI</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2422">OPCUA-2422</a>]
	                  - quasar CI: ignore StandardMetaData for current
	                  (temporary) incompliance with UA data model</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2423">OPCUA-2423</a>]
	                  - Put common parts of CI scripts into block scripts</li>
	              </ul>
	            </main>
	          </td>
	          <td valign="top">quasar test suite notes:<br>
	            <ol>
	              <li>open62541_test_methods not passing for the method
	                arguments behaving differently from UASDK. This will be
	                <a href="https://its.cern.ch/jira/browse/OPCUA-2428">further
	                  investigated</a>.</li>
	              <li> StandardMetaData is ignored from testing in all tests
	                due to some existing noncompliance.<br>
	              </li>
	            </ol>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.4<br>
	            <font size="-1">(22-Jun-2021)</font><br>
	          </td>
	          <td valign="top">Extended coverage in tests and many
	            individual improvements.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2274">OPCUA-2274</a>]
	                - Simplify travis.yml </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2330">OPCUA-2330</a>]
	                - "./quasar.py style_it" missing doc </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2352">OPCUA-2352</a>]
	                - CI, reference nodesets checks for cases:
	                cache_variables, source_variables, methods,
	                config_entries </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2323">OPCUA-2323</a>]
	                - .gitignore in quasar-based servers to go user-managed
	              </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2334">OPCUA-2334</a>]
	                - Unwrackify quasarCommands.html </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2336">OPCUA-2336</a>]
	                - warning: extra ; [-Wpedantic] in
	                ../AddressSpace/include/ASDelegatingMethod.h and few
	                more </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2339">OPCUA-2339</a>]
	                - The output of external_tool_check might be confusing </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2340">OPCUA-2340</a>]
	                - Something fishy about ArrayTools </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2348">OPCUA-2348</a>]
	                - Fix mixed indent/style of Design.xsd </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)<br>
	          </td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.3<br>
	            <font size="-1">(21-May-2021)</font><br>
	          </td>
	          <td valign="top">OPCUA-2312 is needed for fixing Poverty
	            extension module.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2312">OPCUA-2312</a>]
	                - Expose Oracle.get_cache_variable_setter arguments part
	                (useful for Poverty) </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2304">OPCUA-2304</a>]
	                - ArrayTools error when compiling with newer g++ </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2313">OPCUA-2313</a>]
	                - Remove const constraint in PassByValue setters and
	                cleanup </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.2<br>
	            <font size="-1">(13-Apr-2021)</font><br>
	          </td>
	          <td valign="top">The primer to testing quasar (in its CI) via
	            comparing dumped node-sets against reference node-sets.<br>
	            <br>
	            Plus a couple of usual fixes and improvements.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">
	            <main role="main" id="main" class="aui-page-panel-content">
	              <meta http-equiv="content-type" content="text/html;
	                charset=windows-1252">
	              Bug<br>
	              <ul>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2232">OPCUA-2232</a>]
	                  - Design diagram: empty class visualization for
	                  pure-methods class</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2246">OPCUA-2246</a>]
	                  - OutputArguments badly propagated</li>
	              </ul>
	              <br>
	              Task<br>
	              <ul>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-1340">OPCUA-1340</a>]
	                  - --create-certificate fails when server instance is
	                  running (i.e. can't open new connections)</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2182">OPCUA-2182</a>]
	                  - quasar's Utils.h, ASUtils.h and QusarThreadPool.h to
	                  go Quasar managed</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2244">OPCUA-2244</a>]
	                  - Document FreeVariable in config_doc of quasar
	                  servers</li>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2257">OPCUA-2257</a>]
	                  - Apply NodeSetCompare in the CI, primer</li>
	              </ul>
	              <br>
	              Improvement<br>
	              <ul>
	                <li>[<a
	                    href="https://its.cern.ch/jira/browse/OPCUA-2264">OPCUA-2264</a>]
	                  - "All tests passed" message to be printed AFTER all
	                  tests</li>
	              </ul>
	            </main>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.1<br>
	            <font size="-1">(16-Feb-2021)</font><br>
	          </td>
	          <td valign="top">Various fixes and improvements.<br>
	          </td>
	          <td valign="top">Note:<br>
	            <ul>
	              <li>--create-certificate built-in command is getting
	                deprecated. In this version we add a warning whenever it
	                is used. In one of future versions this command might
	                get completely removed.<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2137">OPCUA-2137</a>]
	                - Quasar methods, with device logic, with uncaught
	                exception from inside, can crash server? </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2155">OPCUA-2155</a>]
	                - symlink_runtime_deps broken with style_it branch </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2170">OPCUA-2170</a>]
	                - ./quasar generate config_doc fails </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2171">OPCUA-2171</a>]
	                - async_methods bad casting between OpcUa_StatusCode </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2177">OPCUA-2177</a>]
	                - Missing "Value restrictions" in the config doc </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2186">OPCUA-2186</a>]
	                - ConfigurationDocumentation HTML: missing content </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2121">OPCUA-2121</a>]
	                - Make SERVERCONFIG_LOADER default </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2138">OPCUA-2138</a>]
	                - 'project-wise' astyle invocation (proposal) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2172">OPCUA-2172</a>]
	                - Duplicate CI test "uasdk_test_sync_methods" into the
	                async version </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2086">OPCUA-2086</a>]
	                - Split travis CI tests into limited (open62541-only)
	                and full (open62541 + UASDK) versions </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2162">OPCUA-2162</a>]
	                - Deprecation warning on certificate creation for quasar
	                servers </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2176">OPCUA-2176</a>]
	                - Can detect too old astyle? </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2163">OPCUA-2163</a>]
	                - Fix occasional "unused result", "unused function"
	                warning in quasar builds </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2164">OPCUA-2164</a>]
	                - Server-wide catching of exceptions that are not
	                derived from std::exception </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.5.0<br>
	            <font size="-1">(07-Jan-2021)</font><br>
	          </td>
	          <td valign="top">nebula.B3 merged to classic master. This is
	            the common point from where both main branches start off.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2105">OPCUA-2105</a>]
	                - Fix quasar version format in new release scripts
	                (release_nebula) and branch name back to classic </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2109">OPCUA-2109</a>]
	                - Review quasar.html doc file; plenty of info to be
	                updated </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.B3<br>
	            <font size="-1">(07-Jan-2020)</font></td>
	          <td valign="top">Abandoning home-brew files.txt parser into
	            JSON (this release brings benefits mostly to quasar
	            developers)<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2084">OPCUA-2084</a>]
	                - XML or JSON for future format of original_files.txt
	                and files.txt </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2103">OPCUA-2103</a>]
	                - Document QUASAR_LOG_LEVEL </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2063">OPCUA-2063</a>]
	                - Simplify manage_files module: default properties per
	                directory </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.B2<br>
	            <font size="-1">(04-Jan-2020)</font><br>
	          </td>
	          <td valign="top">Different incremental changes.<br>
	          </td>
	          <td valign="top">
	            <ul>
	              <li>note that with this release, Device/CMakeLists.txt
	                becomes 100% quasar managed. This file will get
	                overwritten with subsequent upgrades. Move your custom
	                stuff AWAY from that file.</li>
	              <li>for the first time, the upgrade process is kind of
	                interactive and if chosen it will modify your Design. We
	                have that feature works fine (and the backup file gets
	                created) but still, be careful.<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2073">OPCUA-2073</a>]
	                - Device's CMakeLists.txt to go quasar-managed </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2088">OPCUA-2088</a>]
	                - Convert original_files from legacy format to json, may
	                imply some clean-up of original_files.txt </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2090">OPCUA-2090</a>]
	                - Automate projectShortName for non being empty </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2065">OPCUA-2065</a>]
	                - UTF-8 characters might not be supported on non-UTF8
	                systems (present in quasar...) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2070">OPCUA-2070</a>]
	                - Python's standard logging for quasar python tooling </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2101">OPCUA-2101</a>]
	                - Add more (optional) logging for address-space
	                construction (would deserve separate log component
	                probably) </li>
	            </ul>
	            <br>
	            Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2091">OPCUA-2091</a>]
	                - Do not add property mapping for single variable nodes
	              </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.B1<br>
	            <font size="-1">(25-Nov-2020)</font><br>
	          </td>
	          <td valign="top">The reason for move between A.x and B.x
	            branch is (not so likely) Design schema backwards-incompat
	            change. In addition we get extensions of CI testing and some
	            smaller improvements.<br>
	            Pay attention to the column on the right, there might be
	            some incompatibilities.<br>
	          </td>
	          <td valign="top">
	            <ul>
	              <li>If you use open62541-compat, you need version 1.3.6 or
	                later. </li>
	              <li>Common/include/Utils.h is finally a 100% quasar file.
	                If (unlikely) you have custom mods to this file, please
	                spawn these as a separate, user-supplied file. Otherwise
	                you will lose your custom changes.<br>
	              </li>
	              <li>In this version it is required that the
	                "projectShortName" field in your Design is non-empty. If
	                you haven't named your project so far, it's time to do
	                it. This name will propagate into automatically
	                generated documentation and in future also to build
	                scripts, etc.<br>
	              </li>
	              <li>Important if you build any <b>shared objects</b> from
	                quasar build system: with nebula.A8 we started requiring
	                CMake version at least 3.10. However that also brings <a
	href="https://cmake.org/cmake/help/v3.4/policy/CMP0065.html">CMP0065</a>.
	                If you build any shared objects within your quasar
	                project build system (e.g. for quasar Poverty, or for
	                Common CAN module, etc ...) you might want to double
	                check if ENABLE_EXPORTS is set on the .so targets. This
	                release provides transitional protection for CMP0065 by
	                setting the behaviour to old, however the old behaviour
	                might be discontinued by CMake in future.</li>
	            </ul>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2074">OPCUA-2074</a>]
	                - CMake required 3.4 (in 3.3 still OK) breaks
	                compatibility for shared objects </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2007">OPCUA-2007</a>]
	                - Travis CI extensions: dump address space </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1775">OPCUA-1775</a>]
	                - Restrict isKey usage to config-entry
	                (backwards-incompatible Design schema change) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2075">OPCUA-2075</a>]
	                - Common/Utils.h to get into quasar control </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2076">OPCUA-2076</a>]
	                - supply default of project short name, force usage. </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2072">OPCUA-2072</a>]
	                - Extend design validation for isKey without device
	                logic </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A10<br>
	            <font size="-1">(18-Nov-2020)</font><br>
	          </td>
	          <td valign="top">A critical bug fixed that prevented an user
	            from using quasar. Added logging of startup info. Other
	            improvements for simplified maintenance of the project.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1668">OPCUA-1668</a>]
	                - Class with different parents leads to compile error </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2048">OPCUA-2048</a>]
	                - Log server startup information (command line args,
	                working directory etc) -- nebula context </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1815">OPCUA-1815</a>]
	                - Improve restriction parsing </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2062">OPCUA-2062</a>]
	                - Simplify manage_files module: use DesignInspector to
	                parse Design </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2066">OPCUA-2066</a>]
	                - Simplify manage_files module: 1. refactoring </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A9<br>
	            <font size="-1">(10-Nov-2020)</font><br>
	          </td>
	          <td valign="top">New CI tests and extensions, colorful global
	            exception messages, methods synchronization support...<br>
	          </td>
	          <td valign="top">If you use open62541-compat, you need version
	            1.3.5 or later. </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1041">OPCUA-1041</a>]
	                - Non-delegated arrays don't respect per-design size
	                constraints </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1259">OPCUA-1259</a>]
	                - Synchronization support for methods </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2049">OPCUA-2049</a>]
	                - Test for config restrictions (part 1: Design) </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2042">OPCUA-2042</a>]
	                - Selective travis CI test </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2050">OPCUA-2050</a>]
	                - Schema restriction: restrictionByPattern's pattern
	                can't by empty </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2052">OPCUA-2052</a>]
	                - Make global exception message more striking (visible)
	              </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A8<br>
	            <font size="-1">(05-Nov-2020)</font><br>
	          </td>
	          <td valign="top">Quasar is now much more consistent on null
	            setters.<br>
	            Also, see the new improvements.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1273">OPCUA-1273</a>]
	                - Use overloads with QUASAR_NULL (or so) data-type to
	                improve null setters </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1770">OPCUA-1770</a>]
	                - quasar.py build, determine chosen builder from the
	                "build" directory if possible </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2033">OPCUA-2033</a>]
	                - Improve travis runner for concurrency </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2038">OPCUA-2038</a>]
	                - quasar to require cmake version at least 3.10 (issues
	                with 3.3 were identified) </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A7<br>
	            <font size="-1">(22-Oct-2020)</font><br>
	          </td>
	          <td valign="top">Whole work done was kind of important: see
	            the right-most column.<br>
	          </td>
	          <td valign="top">If you use open62541-compat, you need version
	            1.3.4 or later.</td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1603">OPCUA-1603</a>]
	                - quasar arrays with config initializer, with value
	                restrictions, do not build </li>
	            </ul>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1971">OPCUA-1971</a>]
	                - Expose configentry as OPC-UA property - for scalars </li>
	            </ul>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2019">OPCUA-2019</a>]
	                - New CI test dedicated to config entries </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2022">OPCUA-2022</a>]
	                - Add a script to run CI test locally </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A6<br>
	            <font size="-1">(13-Oct-2020)</font><br>
	          </td>
	          <td valign="top">Spotlight: major extensions in testing of the
	            project.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2003">OPCUA-2003</a>]
	                - Unable to generate Address Space doc </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1992">OPCUA-1992</a>]
	                - Removal of (home-brew) backup indenter
	                (astyleSubstitute.py) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2002">OPCUA-2002</a>]
	                - Travis CI extensions: run the server(s) after building
	                them. </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-2006">OPCUA-2006</a>]
	                - Travis CI extensions: supply actual config files </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A5<br>
	            <font size="-1">(30-Sep-2020)</font><br>
	          </td>
	          <td valign="top">Just steps forward ...<br>
	          </td>
	          <td valign="top">If you use open62541-compat, you need version
	            1.3.2 or later.<br>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1890">OPCUA-1890</a>]
	                - Fix streaming into astyle </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1988">OPCUA-1988</a>]
	                - Follow-up renaming of raw_input() to input() in
	                Python3 </li>
	            </ul>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1990">OPCUA-1990</a>]
	                - Improve error message for address space construction </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A4<br>
	            <font size="-1">(29-Sep-2020)</font><br>
	          </td>
	          <td valign="top">Service release for OPCUA-1984 mostly. </td>
	          <td valign="top">Certain design files might now get
	            invalidated by the new feature of the validator, OPCUA-1983.<br>
	          </td>
	          <td valign="top">New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1983">OPCUA-1983</a>]
	                - New validator condition: d:object allowed only when
	                "configureByDesign" </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1984">OPCUA-1984</a>]
	                - Multiple return values per method mishandled (was:
	                [OPCUALARLTDB SUPPORT] server don't start
	                BadNodeIdExists) </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A3<br>
	            <font size="-1">(24-Sep-2020)</font><br>
	          </td>
	          <td valign="top">Service releases fixing two issues.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1972">OPCUA-1972</a>]
	                - CLONE - Configurator: regression from 1.3.x to 1.4.1,
	                breaks build </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1969">OPCUA-1969</a>]
	                - Missing colorama dependency in quasar doc </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1981">OPCUA-1981</a>]
	                - Nextgen design diagrams flawed when class is totally
	                empty </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A2<br>
	            <font size="-1">(15-Sep-2020)</font><br>
	          </td>
	          <td valign="top">Mostly: new CI tests.<br>
	            <br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1888">OPCUA-1888</a>]
	                - Test instantiation from design </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1945">OPCUA-1945</a>]
	                - Migrate inspect() calls in quasar.py to P3's
	                equivalent </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1958">OPCUA-1958</a>]
	                - Test recurrent hasObjects </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1959">OPCUA-1959</a>]
	                - Test singleVariableNode </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">nebula.A1<br>
	            <font size="-1">(14-Sep-2020)</font><br>
	          </td>
	          <td valign="top">First release of the nebula branch.<br>
	          </td>
	          <td valign="top">
	            <ul>
	              <li>Python2 is no longer invoked (the call to Python3 is
	                specifically made)</li>
	              <li>nebula no longer has the AppVeyor CI so from now on
	                the basic compatibility against Windows is not
	                guaranteed in the nebula branch,<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1943">OPCUA-1943</a>]
	                - quasar's Python shebang to Python3 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1960">OPCUA-1960</a>]
	                - Removal of unconditional AppVeyor checks in
	                quasar-nebula </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1961">OPCUA-1961</a>]
	                - Fix create_release script for nebula </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1962">OPCUA-1962</a>]
	                - Rework of module compat check: part1: removal </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.4.2<br>
	            <font size="-1">(27-Aug-2020)</font><br>
	          </td>
	          <td valign="top">Last phase of NextGen transitions to Jinja2
	            and additions of FreeVariable concept.<br>
	          </td>
	          <td valign="top">(none known) </td>
	          <td valign="top"> <br>
	            Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1922">OPCUA-1922</a>]
	                - quasar invocation to astyle is not checked for return
	                code </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1718">OPCUA-1718</a>]
	                - Test and merge LogIt master (with change OPCUA-1671 -
	                LCG_96 build) to quasar </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1895">OPCUA-1895</a>]
	                - Add recommendations on whitespace for Jinja transforma
	              </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1896">OPCUA-1896</a>]
	                - FreeVariable concept for quasar </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1897">OPCUA-1897</a>]
	                - Update python_requirements.txt </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1928">OPCUA-1928</a>]
	                - Rework source variables with Jinja </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1332">OPCUA-1332</a>]
	                - add means to determine quasar version from quasar.py </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1760">OPCUA-1760</a>]
	                - Clean-up of device logic classes </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1766">OPCUA-1766</a>]
	                - Improve UA_DISABLE_COPY and private constructor idiom,
	                go to =delete </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1881">OPCUA-1881</a>]
	                - Remove parent struct (C++ code) from DesignInspector </li>
	            </ul>
	            <br>
	            <br>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.4.1<br>
	            <font size="-1">05-Jun-2020</font><br>
	          </td>
	          <td valign="top">Next phase of NextGen transition:
	            improvements, further removal of XSLT. New validator is
	            introduced.<br>
	          </td>
	          <td valign="top">
	            <ul>
	              <li> quasar now features a new Design Validator which is
	                much stronger than the previous solution. It might mean
	                to catch issues in your Designs, even if the servers
	                build just fine.<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1804">OPCUA-1804</a>]
	                - Validate presence of d:mutex when any dependent lock
	                is requested </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1816">OPCUA-1816</a>]
	                - Documentation for array type cacheVars omitted from
	                generated configuration.xsd </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1851">OPCUA-1851</a>]
	                - Wrong text in ConfigDoc.html
	                (designToConfigDocumentationHtml.jinja transform) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1854">OPCUA-1854</a>]
	                - Configuration decoration backwards compatibility
	                broken (since 1.3.6) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1863">OPCUA-1863</a>]
	                - ./quasar.py symlink_runtime_deps does not work with
	                quasar 1.4.1 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1866">OPCUA-1866</a>]
	                - Documentation: broken link for
	                CalculatedVariables.html documentation </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1873">OPCUA-1873</a>]
	                - NextGen configurator fails for GenericSNMP server both
	                with 1.4.0 and pre-1.4.1 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1878">OPCUA-1878</a>]
	                - configuration documentation: missing default values
	                and mandatory/optional </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1879">OPCUA-1879</a>]
	                - AddressSpace classes incorrectly want to initialize
	                from config when 'valueAndStatus' </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1880">OPCUA-1880</a>]
	                - Validator not catching array and initialValue presence
	              </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1855">OPCUA-1855</a>]
	                - atomic makedirs </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1778">OPCUA-1778</a>]
	                - initialValue validation </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1780">OPCUA-1780</a>]
	                - Catch and ban all instantiations from design where
	                there is any config data (config entries or
	                config-initialized cache variables) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1798">OPCUA-1798</a>]
	                - Catch CV array with valueAndStatusInitializer in
	                validateDesign target (now is in AS classes generation)
	              </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1799">OPCUA-1799</a>]
	                - Guarantee initialStatus presence when
	                initializeWith=initialValue in NextGen code gen </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1849">OPCUA-1849</a>]
	                - Translate quasar diagrams generator to Jinja2 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1850">OPCUA-1850</a>]
	                - Revamp design validator </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1758">OPCUA-1758</a>]
	                - Remove deprecated files from ancient times </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1856">OPCUA-1856</a>]
	                - Address Space documentation using NextGen transforms </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1859">OPCUA-1859</a>]
	                - Configure AS/device objects retaining original order
	                from config.xml file using xsdcxx content_order
	                mechanism </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1861">OPCUA-1861</a>]
	                - expose quasar exception handler for 3rd party
	                extensions </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1867">OPCUA-1867</a>]
	                - Better debug for Jinja templates </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1868">OPCUA-1868</a>]
	                - Add docs to writers of transforms and code
	                transformations </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.4.0<br>
	            <font size="-1">04-May-2020</font><br>
	          </td>
	          <td valign="top">First release with NextGen transforms !<br>
	            Note that some transforms are still XSLT based, XSLT will be
	            progressively phased out.<br>
	          </td>
	          <td valign="top">
	            <ul>
	              <li>due to imperfections of quasar, previously quasar's
	                Base Classes (on which you build your Device Logic
	                classes) would include too many header files for you.
	                That means that maybe you never had correct includes in
	                your Device Logic classes. This release is much more
	                correct regarding that aspect so you might need to
	                finally fix that. It's just a matter of adding few
	                missing #includes.<br>
	              </li>
	              <li>in case there is something wrong with the new code
	                generation, you can force quasar to run solely the old
	                transforms, just edit a switch in
	                FrameworkInternals/transformDesign.py called
	                LEGACY_CODE_GENERATION to True.<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top"> <br>
	            Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1804">OPCUA-1804</a>]
	                - Validate presence of d:mutex when any dependent lock
	                is requested </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1816">OPCUA-1816</a>]
	                - Documentation for array type cacheVars omitted from
	                generated configuration.xsd </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1851">OPCUA-1851</a>]
	                - Wrong text in ConfigDoc.html
	                (designToConfigDocumentationHtml.jinja transform) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1854">OPCUA-1854</a>]
	                - Configuration decoration backwards compatibility
	                broken (since 1.3.6) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1863">OPCUA-1863</a>]
	                - ./quasar.py symlink_runtime_deps does not work with
	                quasar 1.4.1 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1866">OPCUA-1866</a>]
	                - Documentation: broken link for
	                CalculatedVariables.html documentation </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1873">OPCUA-1873</a>]
	                - NextGen configurator fails for GenericSNMP server both
	                with 1.4.0 and pre-1.4.1 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1878">OPCUA-1878</a>]
	                - configuration documentation: missing default values
	                and mandatory/optional </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1879">OPCUA-1879</a>]
	                - AddressSpace classes incorrectly want to initialize
	                from config when 'valueAndStatus' </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1880">OPCUA-1880</a>]
	                - Validator not catching array and initialValue presence
	              </li>
	            </ul>
	            <br>
	            New Feature <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1855">OPCUA-1855</a>]
	                - atomic makedirs </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1778">OPCUA-1778</a>]
	                - initialValue validation </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1780">OPCUA-1780</a>]
	                - Catch and ban all instantiations from design where
	                there is any config data (config entries or
	                config-initialized cache variables) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1798">OPCUA-1798</a>]
	                - Catch CV array with valueAndStatusInitializer in
	                validateDesign target (now is in AS classes generation)
	              </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1799">OPCUA-1799</a>]
	                - Guarantee initialStatus presence when
	                initializeWith=initialValue in NextGen code gen </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1849">OPCUA-1849</a>]
	                - Translate quasar diagrams generator to Jinja2 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1850">OPCUA-1850</a>]
	                - Revamp design validator </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1758">OPCUA-1758</a>]
	                - Remove deprecated files from ancient times </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1856">OPCUA-1856</a>]
	                - Address Space documentation using NextGen transforms </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1859">OPCUA-1859</a>]
	                - Configure AS/device objects retaining original order
	                from config.xml file using xsdcxx content_order
	                mechanism </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1861">OPCUA-1861</a>]
	                - expose quasar exception handler for 3rd party
	                extensions </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1867">OPCUA-1867</a>]
	                - Better debug for Jinja templates </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1868">OPCUA-1868</a>]
	                - Add docs to writers of transforms and code
	                transformations </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.14<br>
	            <font size="-2">15-Apr-2020</font><br>
	          </td>
	          <td valign="top">A service release with many minor
	            improvements<br>
	          </td>
	          <td valign="top">
	            <ul>
	              <li>Consider moving your custom sources listed in
	                Device/CMakeLists.txt to a newly added file
	                Device/DeviceCustom.cmake -- Device/CMakeLists.txt will
	                become a file owned by the quasar framework in one of
	                future releases!</li>
	              <li>If you use open62541-compat, please use version 1.2.2
	                or newer.<br>
	              </li>
	            </ul>
	            <br>
	          </td>
	          <td valign="top"> <br>
	            Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1788">OPCUA-1788</a>]
	                - Non-compiling arrays of UaByteString and UaVariant </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1801">OPCUA-1801</a>]
	                - Amalgamation encoding missing, relevant for Python3.6+
	              </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1777">OPCUA-1777</a>]
	                - Signature mismatch for OpcUa_XInt16 setters (X from
	                {null, "U"}) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1787">OPCUA-1787</a>]
	                - Prepare a comprehensive test-design for methods
	                (configurable variant: synchronous vs asynchronous) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1800">OPCUA-1800</a>]
	                - Add UA-SDK based tests to quasar Travis </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1802">OPCUA-1802</a>]
	                - Test source-variables </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1806">OPCUA-1806</a>]
	                - Add setters/getters invocation to test_cache_variables
	                test </li>
	            </ul>
	            <br>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1764">OPCUA-1764</a>]
	                - Split Device's CMakeLists into user and non-user part
	              </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1785">OPCUA-1785</a>]
	                - Lot of stale code to remove in ASNodeManager.cpp </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.13<br>
	            <font size="-1">27-Mar-2020</font><br>
	          </td>
	          <td valign="top">The primary advantage are the improvements
	            done to help build different quasar-based servers in
	            restricted areas (like P1), thanks to URL translation for
	            optional modules etc...<br>
	          </td>
	          <td valign="top">Now, by default, find_package(Boost) is used
	            to resolve Boost libraries.<br>
	            <br>
	            If you want the older behaviour, which implied manually
	            specifying boost paths and libs, you can still do it by
	            defining, in the build config of choice, the following CMake
	            variable:<br>
	            <br>
	            IGNORE_DEFAULT_BOOST_SETUP<br>
	          </td>
	          <td valign="top"> Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1781">OPCUA-1781</a>]
	                - cache-variables of arrays of booleans, initialized
	                from configuration, don't build </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1782">OPCUA-1782</a>]
	                - cache-variables of arrays of uint64, initialized from
	                configuration, don't build </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1783">OPCUA-1783</a>]
	                - multiple root/hasobjects elements referring to same
	                class causes non-compiling code </li>
	            </ul>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1776">OPCUA-1776</a>]
	                - Extend quasar travis for some beyond-trivial tests
	                (e.g. inject few different Designs than span some common
	                scenarios) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1790">OPCUA-1790</a>]
	                - Optimize OptionalModules (separate CMake file) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1791">OPCUA-1791</a>]
	                - OptionalModules URL processing for server URL
	                replacement(s) </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1792">OPCUA-1792</a>]
	                - Use find_package(Boost) for general Boost resolution
	                in quasar </li>
	            </ul>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1741">OPCUA-1741</a>]
	                - boost::lexical_cast -&gt; std::to_string in quasar
	                (subject to gcc 4.8.5 avail) </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.12<br>
	            <font size="-1">26-Feb-2020</font><br>
	          </td>
	          <td valign="top">Service release with different fixes for
	            generic formulas and different other improvements.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">
	            <meta http-equiv="content-type" content="text/html;
	              charset=windows-1252">
	            Bug
	            <ul style="margin: 10px 0px 0px; color: rgb(23, 43, 77);
	              font-family: -apple-system, BlinkMacSystemFont,
	              &quot;Segoe UI&quot;, Roboto, Oxygen, Ubuntu, &quot;Fira
	              Sans&quot;, &quot;Droid Sans&quot;, &quot;Helvetica
	              Neue&quot;, sans-serif; font-size: 14px; font-style:
	              normal; font-variant-ligatures: normal; font-variant-caps:
	              normal; font-weight: 400; letter-spacing: normal; orphans:
	              2; text-align: start; text-indent: 0px; text-transform:
	              none; white-space: normal; widows: 2; word-spacing: 0px;
	              -webkit-text-stroke-width: 0px; background-color: rgb(255,
	              255, 255); text-decoration-style: initial;
	              text-decoration-color: initial;">
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1686"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1686</a>]
	                - quasar tolerates empty name of CalculatedVariable</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1687"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1687</a>]
	                - $applyGenericFormula incorrect behaviour for formula
	                replacement, should be partial, is complete</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1703"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1703</a>]
	                - use python3 in appveyor build</li>
	            </ul>
	            Task
	            <ul style="margin: 10px 0px 0px; color: rgb(23, 43, 77);
	              font-family: -apple-system, BlinkMacSystemFont,
	              &quot;Segoe UI&quot;, Roboto, Oxygen, Ubuntu, &quot;Fira
	              Sans&quot;, &quot;Droid Sans&quot;, &quot;Helvetica
	              Neue&quot;, sans-serif; font-size: 14px; font-style:
	              normal; font-variant-ligatures: normal; font-variant-caps:
	              normal; font-weight: 400; letter-spacing: normal; orphans:
	              2; text-align: start; text-indent: 0px; text-transform:
	              none; white-space: normal; widows: 2; word-spacing: 0px;
	              -webkit-text-stroke-width: 0px; background-color: rgb(255,
	              255, 255); text-decoration-style: initial;
	              text-decoration-color: initial;">
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1286"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1286</a>]
	                - open62541-based server doesn't quit when the
	                initialization failed (e.g. network port was in use)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1683"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1683</a>]
	                - Quasar compilation warning from Configurator</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1697"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1697</a>]
	                - Fix default_configuration</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1705"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1705</a>]
	                - Remove allow_long_disguise for program options</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1726"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1726</a>]
	                - $applyGenericFormula captures wrongly</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1727"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1727</a>]
	                - trivial: wrong command name referenced in exception -
	                build_config</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1732"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1732</a>]
	                - Add defaults for ThreadPool config in the
	                Configuration xsd generator</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.11<br>
	            <font size="-1">06-Jan-2020</font><br>
	          </td>
	          <td valign="top">Stabilization fixes, especially with
	            MetaBuildInfo<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top"> Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1657">OPCUA-1657</a>]
	                - fix windows and linux CI builds for master </li>
	            </ul>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1653">OPCUA-1653</a>]
	                - Fix meta_build_info.py for the case of NO external
	                build libs </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1681">OPCUA-1681</a>]
	                - Check GCC 8.3 warning for string operation truncation
	              </li>
	            </ul>
	            Improvement <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1679">OPCUA-1679</a>]
	                - boost -&gt; std migration (where possible) </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.10<br>
	            <font size="-1">04-Dec-2019</font><br>
	          </td>
	          <td valign="top">Isolates code in Server/ module from
	            internals of open62541, it'd permit more independent
	            development of open62541-compat.<br>
	          </td>
	          <td valign="top">If you use open62541-compat, you need version
	            1.1.5 or later.<br>
	          </td>
	          <td valign="top">
	            <meta http-equiv="content-type" content="text/html;
	              charset=windows-1252">
	            Task
	            <ul style="margin: 10px 0px 0px; color: rgb(23, 43, 77);
	              font-family: -apple-system, BlinkMacSystemFont,
	              &quot;Segoe UI&quot;, Roboto, Oxygen, Ubuntu, &quot;Fira
	              Sans&quot;, &quot;Droid Sans&quot;, &quot;Helvetica
	              Neue&quot;, sans-serif; font-size: 14px; font-style:
	              normal; font-variant-ligatures: normal; font-variant-caps:
	              normal; font-weight: 400; letter-spacing: normal; orphans:
	              2; text-align: start; text-indent: 0px; text-transform:
	              none; white-space: normal; widows: 2; word-spacing: 0px;
	              -webkit-text-stroke-width: 0px; background-color: rgb(255,
	              255, 255); text-decoration-style: initial;
	              text-decoration-color: initial;">
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1642"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1642</a>]
	                - Make quasar independent from UA_Server structure
	                (thus, from open62541 version)</li>
	            </ul>
	            Improvement
	            <ul style="margin: 10px 0px 0px; color: rgb(23, 43, 77);
	              font-family: -apple-system, BlinkMacSystemFont,
	              &quot;Segoe UI&quot;, Roboto, Oxygen, Ubuntu, &quot;Fira
	              Sans&quot;, &quot;Droid Sans&quot;, &quot;Helvetica
	              Neue&quot;, sans-serif; font-size: 14px; font-style:
	              normal; font-variant-ligatures: normal; font-variant-caps:
	              normal; font-weight: 400; letter-spacing: normal; orphans:
	              2; text-align: start; text-indent: 0px; text-transform:
	              none; white-space: normal; widows: 2; word-spacing: 0px;
	              -webkit-text-stroke-width: 0px; background-color: rgb(255,
	              255, 255); text-decoration-style: initial;
	              text-decoration-color: initial;">
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1413"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1413</a>]
	                - quasar to work with Unified Automation 1.7.*</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1623"
	                  style="color: rgb(0, 82, 204); text-decoration: none;">OPCUA-1623</a>]
	                - Quasar fails to compile in C++17 mode</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.9<br>
	            <font size="-1">29-Nov-2019</font><br>
	          </td>
	          <td valign="top">Improvements to ensure Python3 compatibility<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top"> Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1585">OPCUA-1585</a>]
	                - quasar python tooling not compatible with Python3 </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1610">OPCUA-1610</a>]
	                - Fix appveyor build - use python 3 (instead of 2) </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1600">OPCUA-1600</a>]
	                - Converge whole quasar Python code to standardised
	                4-spaces separator </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1604">OPCUA-1604</a>]
	                - RPM quasar module to go optional </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1614">OPCUA-1614</a>]
	                - Converge quasar Python-based s cripting to LF line
	                endings </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1630">OPCUA-1630</a>]
	                - Improve md5 computation (a.k.a. subprocess module has
	                no 'getstatusoutput' (In Python 2.7)) </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.8<br>
	            <font size="-1">11-Nov-2019</font><br>
	          </td>
	          <td valign="top">"Quick fix" for OPCUA-1573<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">
	            <meta http-equiv="content-type" content="text/html;
	              charset=windows-1252">
	            Bug <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1568">OPCUA-1568</a>]
	                - No location printed for CalcVars error "Variable sqrt
	                can't be found. Formula error most likely?" </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1573">OPCUA-1573</a>]
	                - Initializers for array values were forgotten to be
	                allowed in the new Configurator (from quasar 1.3.6) </li>
	            </ul>
	            <br>
	            Task <br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1569">OPCUA-1569</a>]
	                - Document no-whitespace between operation and
	                parenthesis-operand </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.7<br>
	            <font size="-1">01-Nov-2019</font><br>
	          </td>
	          <td valign="top">Different optimizations to optimize server
	            build times<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-739">OPCUA-739</a>]
	                - Meta amalgamation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1546">OPCUA-1546</a>]
	                - Support for Ninja builds</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1558">OPCUA-1558</a>]
	                - Singleton getter in DRoot</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1563">OPCUA-1563</a>]
	                - Does quasar really require cmake3+ ?</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1564">OPCUA-1564</a>]
	                - Disable certificate validity check until OPCUA-1541 is
	                contributed</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1249">OPCUA-1249</a>]
	                - Experiment with amalgamation to boost compile times of
	                quasar servers</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1269">OPCUA-1269</a>]
	                - Try removing Configuration.hxx include in Base header
	                classes</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1528">OPCUA-1528</a>]
	                - misleading build error message</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.6<br>
	            <font size="-1">27-Sep-2019</font><br>
	          </td>
	          <td valign="top">Changed the order of objects instantiation to
	            follow the XML config file.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1506">OPCUA-1506</a>]
	                - Uncovered case of singular hasObjects</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1523">OPCUA-1523</a>]
	                - SourceVariables read handler, wrong generation</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1495">OPCUA-1495</a>]
	                - Get rid of unused variable warning</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1497">OPCUA-1497</a>]
	                - Support varied-level parents by "thisObject" operator</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1500">OPCUA-1500</a>]
	                - Fix order of instantion to follow the order of XML</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1514">OPCUA-1514</a>]
	                - New configurator : tag name comparison should be
	                case-insensitive</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.5<br>
	            <font size="-1">13-Sep-2019</font><br>
	          </td>
	          <td valign="top">Small improvements and fixes, please refer to
	            the right-most column for details.<br>
	          </td>
	          <td valign="top">If you use m_addressSpaceLink anywhere in
	            your DeviceLogic code, please use getAddressSpaceLink()
	            instead (due to OPCUA-1489).<br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1429">OPCUA-1429</a>]
	                - upgrade_design fails</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1434">OPCUA-1434</a>]
	                - MetaBuildInfo might fail with escaping text and make
	                project impossible to build</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1416">OPCUA-1416</a>]
	                - Different Graphviz layouts for quasar design diagrams</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1426">OPCUA-1426</a>]
	                - Make a pointer from ProjectSettings and
	                build_configuration to the documentation of quasar build
	                system</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1428">OPCUA-1428</a>]
	                - Fix documentation for "./quasar.py upgrade_design"</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1452">OPCUA-1452</a>]
	                - Modernize OpcServer class implementation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1472">OPCUA-1472</a>]
	                - Optimize CalculatedVariables when referring "this"
	                from the formula</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1480">OPCUA-1480</a>]
	                - quasar w/open62541 doesn't build</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1481">OPCUA-1481</a>]
	                - Resolve warnings and deprecated functions in quasar
	                servers</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1487">OPCUA-1487</a>]
	                - Suppress StandardMetaData / connectedClientCount</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1489">OPCUA-1489</a>]
	                - Force usage of getAddressSpaceLink by finally making
	                m_addressSpaceLink non-public</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1499">OPCUA-1499</a>]
	                - Fix docs for Yocto builds</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1425">OPCUA-1425</a>]
	                - Update quasar LogIt submodule reference to collect
	                'append' configuration for boost log</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1504">OPCUA-1504</a>]
	                - MetaBuildInfo: improve the contents of meta
	                information 'commitID'</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.4<br>
	            <font size="-1">16-Apr-2019</font><br>
	          </td>
	          <td valign="top">Support for config-entry defaults and various
	            smaller improvements.<br>
	          </td>
	          <td valign="top">If you UA SDK, please update your build
	            config files. You should add the following includes:<br>
	            <br>
	            include_directories (<br>
	            ${OPCUA_TOOLKIT_PATH}/include/uastack<br>
	            ${OPCUA_TOOLKIT_PATH}/include/uabase<br>
	            ${OPCUA_TOOLKIT_PATH}/include/uaserver<br>
	            ${OPCUA_TOOLKIT_PATH}/include/xmlparser<br>
	            ${OPCUA_TOOLKIT_PATH}/include/uapki<br>
	            )<br>
	            <br>
	            This is due to clean-up effort originating from OPCUA-1278.<br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1336">OPCUA-1336</a>]
	                - symlink_runtime_deps is broken on windows</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1353">OPCUA-1353</a>]
	                - Return value of UA_Server_run not taken into account</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1370">OPCUA-1370</a>]
	                - SourceVariables asynchronous with type UaString is
	                misbehaving</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1371">OPCUA-1371</a>]
	                - Valid but non-compiling Design for source-variables on
	                class without Device Logic</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1376">OPCUA-1376</a>]
	                - Incorrect population of ArrayDimensions, forbids
	                arrays to be used with open62541 0.3</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1380">OPCUA-1380</a>]
	                - array cache-vars wrongly propagate to config with no
	                configInitializer</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-879">OPCUA-879</a>]
	                - Document Quasar build system</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1302">OPCUA-1302</a>]
	                - Remove (already deprecated) support for open62541 0.2
	                in o6-compat</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1314">OPCUA-1314</a>]
	                - Creation of runtime symlinks dependencies by default</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1335">OPCUA-1335</a>]
	                - quasar RPM builder: various improvements</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1278">OPCUA-1278</a>]
	                - Move backend specific header inclusions to relevant
	                build configs</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1321">OPCUA-1321</a>]
	                - Lower severity for Log message when NULL value can't
	                be taken for CalculatedVariable evaluation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1375">OPCUA-1375</a>]
	                - Improve Optional Modules message</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1381">OPCUA-1381</a>]
	                - Support for config-entry defaults</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1397">OPCUA-1397</a>]
	                - Add windows debugger console sink to LogIt</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.3<br>
	            <font size="-1">16-Jan-2019</font><br>
	          </td>
	          <td valign="top">Features needed for embeddability into Python
	            or other languages<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1284">OPCUA-1284</a>]
	                - Let external classes access node managers (relevant
	                e.g. for embedding quasar into Python)</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1274">OPCUA-1274</a>]
	                - Support for quasar for building servers as shared
	                objects</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1275">OPCUA-1275</a>]
	                - Unify quasar "shutdown" ecosystem among different
	                backends</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.2<br>
	            <font size="-1">13-Dec-2018</font><br>
	          </td>
	          <td valign="top">Introducing CalculatedVariables<br>
	          </td>
	          <td valign="top">Inspect your Server/src/QuasarServer.cpp<br>
	            The method QuasarServer::initializeLogIt() should do:<br>
	            <br>
	            BaseQuasarServer::initializeLogIt();<br>
	            <br>
	            rather than directly call LogIt initialize functions.
	            Otherwise you will not get any logging from quasar-defined
	            logging components, like CalcVars.<br>
	          </td>
	          <td valign="top">Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-696">OPCUA-696</a>]
	                - Calculated variables</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1265">OPCUA-1265</a>]
	                - Support for CalculatedVariables in config doc
	                generation</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.1<br>
	            <font size="-1">05-Dec-2018</font><br>
	          </td>
	          <td valign="top">The release features BuildMetaInfo.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1148">OPCUA-1148</a>]
	                - BuildMetaInfo, quasar-standard support for
	                build-related information via address-space and
	                command-line</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1216">OPCUA-1216</a>]
	                - transformDesign() support for multiple additional
	                parameters</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1234">OPCUA-1234</a>]
	                - Unfreed memory in open62541 server config object</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.3.0<br>
	            <font size="-1">15-Nov-2018</font><br>
	          </td>
	          <td valign="top">The build system is the primary actor of this
	            release:<br>
	            <ul>
	              <li>CMake3 is now required,</li>
	              <li>out-of-source builds are now default<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">If you use UASDK (or any other backend that
	            comes built before quasar server gets build) rather than
	            open62541-compat, you should add a virtual target in your
	            build config file:<br>
	            <br>
	            add_custom_target( quasar_opcua_backend_is_ready )<br>
	            <br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1213">OPCUA-1213</a>]
	                - ASInformationModel generation not dependent (in CMake
	                sense) on the xslt</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1016">OPCUA-1016</a>]
	                - Yocto+quasar </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1165">OPCUA-1165</a>]
	                - Yocto branch upmerge</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1183">OPCUA-1183</a>]
	                - cmake3 in quasar projects</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1186">OPCUA-1186</a>]
	                - Compatibility for open62541 0.3 for
	                opcserver_open62541.cpp</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1193">OPCUA-1193</a>]
	                - Fix documentation on quasar build for Ubuntu 18
	                (FetchContent issues)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1211">OPCUA-1211</a>]
	                - Convenience quasar function to prepare environment in
	                build/bin</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1109">OPCUA-1109</a>]
	                - Make out-of-source build default</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.7<br>
	            <font size="-1">25-Oct-2019</font><br>
	          </td>
	          <td valign="top">This release is mostly oriented on the
	            deployment of new version of LogIt and the change in which
	            LogIt source code is maintained. A number of improvements
	            have been implemented as well.<br>
	          </td>
	          <td valign="top">After checking out the tag 1.2.7 in the
	            quasar repo, due to the fact of moving LogIt to a separate
	            github repo, you should execute:<br>
	            <br>
	            <br>
	            <pre style="margin: 0; line-height: 125%">git submodule update --init --recursive</pre>
	            <br>
	            For all users who used to profit from <b>UATRACE</b>
	            logging sink: you are requested to add to your build config
	            the following line:<br>
	            <br>
	            <meta http-equiv="content-type" content="text/html;
	              charset=windows-1252">
	            <pre style="font-size: 13.3333px; color: rgb(0, 0, 0); font-style: normal; font-variant-ligatures: normal; font-variant-caps: normal; font-weight: normal; letter-spacing: normal; orphans: 2; text-align: start; text-indent: 0px; text-transform: none; widows: 2; word-spacing: 0px; -webkit-text-stroke-width: 0px; text-decoration-style: initial; text-decoration-color: initial; background-color: rgb(244, 245, 247); margin-top: 0px; margin-bottom: 0px; padding: 0px;"><font color="#172B4D" size="1"><span style="font-size: 12px; background-color: rgb(244, 245, 247);">option(LOGIT_BACKEND_UATRACE <font color="#009100">"Whether to build with UATRACE (logging via UASDK logger)"</font> ON) </span></font></pre>
	            <br>
	            <br>
	            <a href="LogIt.html#maintainers">Read more about LogIt
	              changes</a> <br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1123">OPCUA-1123</a>]
	                - check_consistency should not complain about generated
	                address space classes</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1150">OPCUA-1150</a>]
	                - UaVariant related AddressSpace generation - generates
	                non-compiling code</li>
	            </ul>
	            <br>
	            New Feature<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1131">OPCUA-1131</a>]
	                - Range (min/max) restrictions in config restrictions</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1132">OPCUA-1132</a>]
	                - Add documentation option to source variables</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1139">OPCUA-1139</a>]
	                - Address space documentation creation in analogy to
	                config doc</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1128">OPCUA-1128</a>]
	                - generate device --all doesn't work in SCA OPC-UA</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1144">OPCUA-1144</a>]
	                - Remove stale boost/foreach includes in Meta</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1147">OPCUA-1147</a>]
	                - Compilation error for sourcevariable but without
	                device logic</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1151">OPCUA-1151</a>]
	                - Add d:documentation to methods' arguments and
	                returnvalues</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1153">OPCUA-1153</a>]
	                - LogIt CMakeFiles.txt needs separate paths for
	                standalone vs non-standalone</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-853">OPCUA-853</a>]
	                - Range-based for adoption</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1140">OPCUA-1140</a>]
	                - Rationalise LogIt source code maintenance - quasar to
	                obtain LogIt source code from the dedicated stand-alone
	                LogIt repository.</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1141">OPCUA-1141</a>]
	                - Replace call to system::abort in meta with something
	                more user friendly - throw an exception containing an
	                error message</li>
	            </ul>
	            <a name="editarea"></a> </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.6<br>
	            <font size="-1">21-Aug-2018</font><br>
	          </td>
	          <td valign="top">Major features:<br>
	            <ul>
	              <li>Out-of-source builds</li>
	              <li>XSD restrictions support</li>
	            </ul>
	            <p>Minor features<br>
	            </p>
	            <ul>
	              <li>Generating Config Schema documentation<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">(none known)</td>
	          <td valign="top">New Feature<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-856">OPCUA-856</a>]
	                - XSD restrictions support for Quasar</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1119">OPCUA-1119</a>]
	                - Documenting Config Schema</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1006">OPCUA-1006</a>]
	                - Try to include quasar project as a CMake subproject</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1007">OPCUA-1007</a>]
	                - Out-of-source build capability</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1012">OPCUA-1012</a>]
	                - Rework argument passing into internal quasar functions</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1113">OPCUA-1113</a>]
	                - Abandon out-of-source build if there is CMakeCache
	                present in-source</li>
	            </ul>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1126">OPCUA-1126</a>]
	                - Improve the config doc generator for types of values
	                (including restrictions)</li>
	            </ul>
	            <ul>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.5<br>
	            <font size="-1">06-Aug-2018</font><br>
	          </td>
	          <td valign="top">This is mostly a service release bringing
	            significant simplifications in internal quasar scripting for
	            features to come in future.<br>
	          </td>
	          <td valign="top">The CMake target to generate
	            Configuration.hxx has been renamed to
	            Configuration.hxx_GENERATED .<br>
	            If you used this target e.g. for add_dependencies()
	            statement then you should follow the change accordingly.<br>
	            <br>
	            Please also note that @makeSetGet attribute of
	            cachevariables finally disappears at it has been deprecated
	            since 2014.<br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1036">OPCUA-1036</a>]
	                - isKey doesn't work without device logic and with
	                arrays</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1061">OPCUA-1061</a>]
	                - Target Configuration.hxx confuses the ninja build
	                system, rename it</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1103">OPCUA-1103</a>]
	                - Return code of a quasar server is zero despite an
	                exception caught in BaseQuasarServer</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1097">OPCUA-1097</a>]
	                - Expose device logic mutex via getLock()</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1106">OPCUA-1106</a>]
	                - Simplify Python scripting for running Quasar code
	                generation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1107">OPCUA-1107</a>]
	                - remove @makeSetGet</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1111">OPCUA-1111</a>]
	                - Embedded commands help into quasarCommands.html</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.4<br>
	            <font size="-1">04-Jun-2018</font><br>
	          </td>
	          <td valign="top">Major features:<br>
	            <ul>
	              <li>Asynchronous methods</li>
	              <li>Quasar's thread pool<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">Methods have a new compulsory attribute
	            called "executionSynchronicity". Please pick "synchronous"
	            for the old behaviour. Picking asynchronous will execute a
	            method in a separate context, as per OPCUA-1034.<br>
	            <br>
	            On some systems (e.g. Ubuntu) the following addition in your
	            custom build config might be necessary:<br>
	            <br>
	            <span class="blob-code-inner"><span class="pl-c1">SET</span>(
	              QUASAR_SERVER_LIBS <span class="pl-s">"-lssl -lcrypto
	                -lpthread"</span> )<br>
	              <br>
	              otherwise you might get a message about unmet link
	              dependency to libpthread.<br>
	            </span> </td>
	          <td valign="top">New Feature<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1033">OPCUA-1033</a>]
	                - Methods having array arguments and return values in
	                quasar servers</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1034">OPCUA-1034</a>]
	                - Asynchronous methods</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1009">OPCUA-1009</a>]
	                - Source variables not scaling well in certain types of
	                use cases</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1051">OPCUA-1051</a>]
	                - Remove quasarGUI.py</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1056">OPCUA-1056</a>]
	                - Improve memory management in Certificate.cpp</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1031">OPCUA-1031</a>]
	                - Improve designToConfigurationXSD.xslt post arrays
	                merge</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1044">OPCUA-1044</a>]
	                - Move variant-vector converters from
	                designToClassBody.xslt to CommonFunctions</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.3<br>
	            <font size="-1">04-May-2018</font><br>
	          </td>
	          <td valign="top">Major features:<br>
	            <ul>
	              <li>Arrays in quasar servers</li>
	            </ul>
	            <p>Minor features:<br>
	              <br>
	            </p>
	            <ul>
	              <li>Bug fixes<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">(none known)</td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-990">OPCUA-990</a>]
	                - cache-variable of "UaByteString" and "UaVariant" types
	                don't compile</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1018">OPCUA-1018</a>]
	                - name attribute of any quasar object in config file
	                permitted to be empty string</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1030">OPCUA-1030</a>]
	                - Array delegator for UaString implies known data type
	                so xsl:choose should not be used</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1032">OPCUA-1032</a>]
	                - quasar should remove a partially generated file in
	                case generation fails</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-915">OPCUA-915</a>]
	                - Diagram generation support for arrays</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1023">OPCUA-1023</a>]
	                - integration branch for quasar arrays integration</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1024">OPCUA-1024</a>]
	                - Optimize ArrayTools</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1025">OPCUA-1025</a>]
	                - OneToMaxLimits -&gt; ArraySizeLimits</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1027">OPCUA-1027</a>]
	                - Improve designToClassBody.xslt post arrays merge</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1028">OPCUA-1028</a>]
	                - Move array bounds validation from Design.xml from
	                designToClassBody.xslt to Design.xsd or to 2nd stage
	                validator</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1046">OPCUA-1046</a>]
	                - Cannot document methods in Design.xml</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.2<br>
	            <font size="-1">27-Mar-2018</font><br>
	          </td>
	          <td valign="top">Please have a look at release note for
	            summary.<br>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-923">OPCUA-923</a>]
	                - ProjectSettings should be included after Build config</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-1001">OPCUA-1001</a>]
	                - Sourcevariable datatype declared as float is listed as
	                Variant (type 24) in the address-space</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-994">OPCUA-994</a>]
	                - Relax case restrictions for variable names in quasar</li>
	            </ul>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-854">OPCUA-854</a>]
	                - Optional module version check fails with 'release
	                candidate' tags</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.1<br>
	            <font size="-1">09-Nov-2017</font><br>
	          </td>
	          <td valign="top">Bug fixes:<br>
	            <ul>
	              <li>the mode to create PKI certificates and exit didn't
	                work with UA SDK 1.5.x&nbsp; - fixed</li>
	              <li>honkyTonk didn't work<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">(none known)<br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-873">OPCUA-873</a>]
	                - honkytonk no longer works</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-858">OPCUA-858</a>]
	                - --create-certificate misleading, no warning when
	                certificate already exists, plus possibly it doesnt work
	                with tkit 155</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-874">OPCUA-874</a>]
	                - Improve honkyTonk, generate only when devicelogic
	                present, warn when not</li>
	            </ul>
	            <a name="editarea"></a> </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td valign="top">1.2.0<br>
	            <font size="-1">03-Aug-2017</font><br>
	          </td>
	          <td valign="top">Major features:<br>
	            <ul>
	              <li>Methods support (UASDK + open62541)<br>
	              </li>
	            </ul>
	            <p>Minor features:<br>
	            </p>
	            <ul>
	              <li>astyle compatibility mode</li>
	              <li>sticky build config</li>
	              <li>commonalization of master CMakeLists.txt<br>
	              </li>
	            </ul>
	          </td>
	          <td valign="top">
	            <ol>
	              <li>Note that since this release, the master CMakeLists is
	                "one-size-fits-all", i.e. you should customize it.
	                Instead, your per-project settings like the name of the
	                executable and so on should go to ProjectSettings.cmake,
	                which will be included after the upgrade.<br>
	                <br>
	              </li>
	              <li>Please note that you no longer append your build
	                config file name to "./quasar.py build".&nbsp; Now you
	                should do:<br>
	                ./quasar.py set_build_config
	                &lt;path_to_your_build_config&gt;<br>
	              </li>
	            </ol>
	            <br>
	          </td>
	          <td valign="top">Bug<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-662">OPCUA-662</a>]
	                - prepare.py (from open62541-compat) does not work</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-745">OPCUA-745</a>]
	                - Asynchronous source variable with UaString doesn't
	                compile</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-800">OPCUA-800</a>]
	                - Cosmetic fixes for methods</li>
	            </ul>
	            <br>
	            New Feature<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-697">OPCUA-697</a>]
	                - ByteStream data type</li>
	            </ul>
	            <br>
	            Task<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-106">OPCUA-106</a>]
	                - Quasar methods</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-579">OPCUA-579</a>]
	                - Easily selectable OPC-UA backend configuration file
	                (ServerConfig.xml)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-668">OPCUA-668</a>]
	                - Post-merge: missing copyright header in
	                designToHonkyTonk</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-786">OPCUA-786</a>]
	                - astyle equivalent with no/minimal deployment cost</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-830">OPCUA-830</a>]
	                - Commonalize master CMakeLists.txt, find what specific
	                extensions people have</li>
	            </ul>
	            <br>
	            Improvement<br>
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-689">OPCUA-689</a>]
	                - Optional modules should work from branches too</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-799">OPCUA-799</a>]
	                - Sticky build config</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-819">OPCUA-819</a>]
	                - Model visualization improvements</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.8<br>
	            <font size="-1">18-Jul-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Optional modules</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>Python-based scripting clean-up and enhancement
	                (external_tool_check, etc)<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">In the Design.xsd schema,
	            vendor attribute is now renamed to author. If you used that
	            attribute, please update your Design.xml accordingly.<br>
	          </td>
	          <td style="vertical-align: top;">Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-640">OPCUA-640</a>]
	                - Compilation error with UA API 1.5.x</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-634">OPCUA-634</a>]
	                - Add option to enable_module which would persistify the
	                selection in the VCS used</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-635">OPCUA-635</a>]
	                - Optional modules</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-646">OPCUA-646</a>]
	                - Improve quasar generated headers for device, rename
	                vendor to author</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-583">OPCUA-583</a>]
	                - Python scripting system better error handling and
	                messages, refactoring, and other improvements</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.7<br>
	            <font size="-1">17-Jun-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Compatibility with the API of OPC-UA Toolkit 1.3.x,
	                1.4.x and 1.5.x.<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN<br>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-611">OPCUA-611</a>]
	                - Debug builds (sometimes) abort on startup during meta
	                configuration</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-616">OPCUA-616</a>]
	                - Bug found in shutdown.cpp, affecting certain versions
	                of Windows</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-618">OPCUA-618</a>]
	                - ConfigValidator missing inclusion of Utils.h</li>
	            </ul>
	            New Feature
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-617">OPCUA-617</a>]
	                - check_consistency working with projects stored on git</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-588">OPCUA-588</a>]
	                - Port quasar to OPC-UA Toolkit 1.4.x</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-606">OPCUA-606</a>]
	                - Refresh FAQ of Quasar</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-627">OPCUA-627</a>]
	                - Improve VCS ignore lists</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.6<br>
	            <font size="-1">29-Apr-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Major and minor features,
	            look into the rightmost column for details.<br>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN<br>
	          </td>
	          <td style="vertical-align: top;"> New Feature
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-608">OPCUA-608</a>]
	                - Ability to use server LogIt instance for logging from
	                within dynamically loaded libraries on linux and windows</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-560">OPCUA-560</a>]
	                - Configuration discovery/serialization</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-575">OPCUA-575</a>]
	                - Application-specific command line options</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-594">OPCUA-594</a>]
	                - RPM building in Quasar: Create an analogue of
	                build_from_svn for git projects</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-600">OPCUA-600</a>]
	                - Clarify situation of Saxon jar inclusion; if
	                impossible find alternative deployment</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.5<br>
	            <font size="-1">29-Jan-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Minor fixes:<br>
	            <ul>
	              <li>Missing copyright headers, and a small fix that
	                wouldn't compile in proper C++11 compiler<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN<br>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-577">OPCUA-577</a>]
	                - gcc4.8.2 confused by const of NodeManager</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-574">OPCUA-574</a>]
	                - Missing copyright header in ASNodeQueries.h</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.4<br>
	            <font size="-1">27-Jan-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Supplied MSVC (Windows) configuration now defaults to
	                MSVC2013</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>Fix for cache variables with isKey=true and UaString
	                dataType<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">sys/time.h and uadatetime.h
	            are no longer included Utils.h.<br>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-565">OPCUA-565</a>]
	                - Using configentry with dataType "UaString" as key
	                (attribute isKey="true") generates non-compiling Base_D*
	                code</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-566">OPCUA-566</a>]
	                - QUASAR for windows: Migration from VC++ 2010 to VC++
	                2013</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.3<br>
	            <font size="-1">27-Jan-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Single-item-classes (where the only variable takes
	                place of the object in the Address Space)</li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">None known<br>
	          </td>
	          <td style="vertical-align: top;"> Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-540">OPCUA-540</a>]
	                - Unified device parent in DeviceLogic no matter if the
	                factual parent exists or not</li>
	            </ul>
	            New Feature
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-489">OPCUA-489</a>]
	                - Make QUASAR framework version info available to OPC-UA
	                clients from StandardMetaData</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-555">OPCUA-555</a>]
	                - Single-item-class support (a.k.a. 'primitive')</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-557">OPCUA-557</a>]
	                - Move heavy (templated) code for node search from
	                ASNodeManager.h into separate header</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-561">OPCUA-561</a>]
	                - Better level of detail for visualtion (generate
	                diagram) to see presence of device lgoic</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.2<br>
	            <font size="-1">27-Jan-2016</font><br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Re-design of main.cpp: it should since this revision
	                be unchanged by the developer. Custom parts should go
	                into QuasarServer.cpp<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">It is required to migrate to
	            your main.cpp into new QuasarServer.cpp<br>
	            <br>
	            Your initialization code should go to
	            QuasarServer::initialize(),<br>
	            your shutdown code should go to QuasarServer::shutdown(),<br>
	            and the main loop into QuasarServer::mainLoop().<br>
	            <br>
	            <br>
	          </td>
	          <td style="vertical-align: top;"> Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-447">OPCUA-447</a>]
	                - Improve main.cpp to clearly structure it</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-527">OPCUA-527</a>]
	                - New standard meta data</li>
	            </ul>
	            New Feature
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-489">OPCUA-489</a>]
	                - Make QUASAR framework version info available to OPC-UA
	                clients from StandardMetaData</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-519">OPCUA-519</a>]
	                - Fix copyright info - LogIt, Meta</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-522">OPCUA-522</a>]
	                - Add License.txt with GPL license into Quasar sources</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-532">OPCUA-532</a>]
	                - Add header for files.txt and original_files.txt</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.1<br>
	            <font size="-1">19-Nov-2015</font><br>
	          </td>
	          <td style="vertical-align: top;">Minor features:<br>
	            <ul>
	              <li>Python commands got renamed to what was chosen by
	                Quasar developers as most optimial<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">framework.py -&gt; quasar.py<br>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-529">OPCUA-529</a>]
	                - upgrade_design parameters not forwarded (effectively
	                not working)</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-526">OPCUA-526</a>]
	                - Python commands renaming</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.1.0<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Big clean-up which brings awaited feature of
	                converting "deviceLogicTypeName" into something human
	                (but breaks Design.xml compatibility at the same time)</li>
	              <li>Decoupling Base device classes from the address space
	                completely</li>
	              <li>Proper headers now introduced in almost anything apart
	                from Meta, LogIt and Python scripts</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>Clean-up of source variables logging -&gt; now using
	                LogIt</li>
	              <li>Added in-line (i.e. Eclipse compatible) documentation
	                to source variables attributes<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">Design.xml compatibility
	            likely to be broken in your case (if you use Device Logic),
	            but this can be easily fixed, just run:<br>
	            <br>
	            ./quasar.py upgrade_design convert_to_hasDeviceLogic=yes<br>
	            <br>
	            Also bear in mind that if upgrading from a version earlier
	            than 1.0.2, namespaces have to get fixed (look at notes in
	            1.0.2)<br>
	          </td>
	          <td style="vertical-align: top;"> Sub-task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-512">OPCUA-512</a>]
	                - Dissociate base from Address Space completely</li>
	            </ul>
	            Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-513">OPCUA-513</a>]
	                - Shipped config.xml and Design.xml still have old XML
	                namespace</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-514">OPCUA-514</a>]
	                - deviceLogicTypeName -&gt; hasDeviceLogic</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-515">OPCUA-515</a>]
	                - Fix copyright info - Piotr's code</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-516">OPCUA-516</a>]
	                - In-line documentation to attributes of source
	                variables</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-517">OPCUA-517</a>]
	                - upgrade_design doesn't take arguments</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-518">OPCUA-518</a>]
	                - Fix the namespace for CommonFunctions</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-520">OPCUA-520</a>]
	                - Source variables should be logged using LogIt</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.0.2<br>
	          </td>
	          <td style="vertical-align: top; text-align: left;">Minor
	            features:<br>
	            <ul>
	              <li>A feature to store configentry data permanently in the
	                Device object.<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">Is is required to replace in
	            your Design.xml the occurencies of:<br>
	            <br>
	            "http://www.example.org/Design"<br>
	            <br>
	            to<br>
	            <br>
	            "http://cern.ch/quasar/Design"<br>
	            <br>
	            You can automate it using sed:<br>
	            sed -e 's/www.example.org/cern.ch\/quasar/g' Design.xml -i<br>
	            <br>
	            same for configuration files.<br>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-504">OPCUA-504</a>]
	                - Problem with pythonized scripts</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-505">OPCUA-505</a>]
	                - check_consistency fails</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-506">OPCUA-506</a>]
	                - Synchronous source variable not properly generated in
	                Device</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-495">OPCUA-495</a>]
	                - Design.XSD Protection for unique names of variables
	                within a class</li>
	            </ul>
	            New Feature
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-482">OPCUA-482</a>]
	                - (optional) storage of &lt;configentry&gt; values, in
	                generated Device classes, provides 'raw' configuration
	                values at runtime.</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-119">OPCUA-119</a>]
	                - Fix example.org namespace </li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.0.1<br>
	          </td>
	          <td style="vertical-align: top;">Minor features:<br>
	            <ul>
	              <li>Supported data types collection has grown to: Int16,
	                UInt16, Int8(=SByte), UInt8(=Byte)<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;"><br>
	          </td>
	          <td style="vertical-align: top;"> Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-488">OPCUA-488</a>]
	                - Keep msbuild specific instructions in msbuild build
	                configuration</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-501">OPCUA-501</a>]
	                - frameworkGUI improved logging</li>
	            </ul>
	            New Feature
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-486">OPCUA-486</a>]
	                - Datatype UINT16 for QUASAR</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">1.0.0<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>sh build scripts converted to Python, so the
	                development and build may be started on any operating
	                system (previously Linux only)</li>
	              <li>MSVC10 compatibility is now guaranteed for delivered
	                and generated code</li>
	            </ul>
	            <span style="font-weight: bold;">Note</span> that versioning
	            schema has changed since this release into more common
	            3-section version id. Otherwise it would be more logical to
	            continue with 0.100 after 0.99 ;-)<br>
	          </td>
	          <td style="vertical-align: top;">With new multiplatform
	            scripts, the entry points of the framework have changed.
	            I.e. there is no longer automated_build_linux.sh script.<br>
	            <br>
	          </td>
	          <td style="vertical-align: top;"> Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-487">OPCUA-487</a>]
	                - Improve #ifdefs in main.cpp to include at least one
	                section or fail if impossible to do so</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-363">OPCUA-363</a>]
	                - transformDesign in python</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-364">OPCUA-364</a>]
	                - Documentation how to install astyle, kdiff3, (possibly
	                Java), python, on Windows (update in Documentation.html)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-483">OPCUA-483</a>]
	                - Windows-enabled Generic Server</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.99<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Cleaned up build system, which allows for
	                customization of paramteres like boost version and path,
	                OPC UA toolkit, custom compiler etc</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>In-line documentation in the Design and in the
	                (generated)Configuration schema</li>
	              <li>Esthetical improvements with LogIt and Meta</li>
	              <li>Many small useful fixes<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">
	            <ul>
	              <li>The structure of CMakeLists.txt have changed:</li>
	              <ul>
	                <li>Server/CMakeLists.txt got standardised, it is no
	                  longer supposed to be maintained by the user</li>
	                <li>It is the master CMakeLists.txt which now have the
	                  statement to create the exec and link appropriate
	                  libraries</li>
	                <li>Therefore it may be necessary to put some effort
	                  into resolving CMakeLists.txt business</li>
	                <li>One may need to prepare custom build configuration
	                  file to capture their specific build instructions
	                  (i.e. custom boost)<br>
	                </li>
	              </ul>
	            </ul>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-387">OPCUA-387</a>]
	                - Bug found in official framework release, file
	                designToGeneratedCmakeDevice.xslt</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-428">OPCUA-428</a>]
	                - Error messages from Meta assume the config file is
	                called config.xml</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-450">OPCUA-450</a>]
	                - No parent device is generated in Base class under
	                certain conditions</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-220">OPCUA-220</a>]
	                - in-line documentation as to what is meant by
	                [forbidden|delegated|regular] for addressSpaceWrite</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-429">OPCUA-429</a>]
	                - Catch global exceptions</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-434">OPCUA-434</a>]
	                - Print component name instead of numerical id when
	                changing log level from the address space</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-441">OPCUA-441</a>]
	                - Include empty Design.xml and config.xml in the
	                framework installation package</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-446">OPCUA-446</a>]
	                - DRoot camelCase for objects</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-448">OPCUA-448</a>]
	                - Inline documentation for all attributes of
	                cachevariable</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-449">OPCUA-449</a>]
	                - Print class and variable for error "For valueAndStatus
	                initializer initialStatus must be given."</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-458">OPCUA-458</a>]
	                - Try carrying documentation over from Design file to
	                Configuration schema</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-166">OPCUA-166</a>]
	                - Allow no device logic for children when parent has one</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-357">OPCUA-357</a>]
	                - Check with evaluation version of OPC UA toolkit;
	                mention in tutorial+docs; prepare build target</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-375">OPCUA-375</a>]
	                - Master CMakeLists: toolkit location should be
	                specified with one CMake variable (defined at the top)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-377">OPCUA-377</a>]
	                - LogIt, use 4 digit year in timestamp</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-385">OPCUA-385</a>]
	                - Proposal of enhancement of the build system to nicely
	                capture OS differences and possibly non-standard
	                components (e.g. boost)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-396">OPCUA-396</a>]
	                - Better end-user error message if trying to start new
	                server instance when server is already running</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-430">OPCUA-430</a>]
	                - Prepare default config when build configuration wasnt
	                specified</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-439">OPCUA-439</a>]
	                - Investigate write/read through the same variable
	                feature</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-440">OPCUA-440</a>]
	                - Fix "Discovery Server not available" currently logged
	                in all OPC UA srv logs</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.98<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>LogIt</li>
	              <li>Meta</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>Thread pool size is not hardcoded (and can be
	                configured from the config file)<br>
	              </li>
	              <li>Data Type identifier field is&nbsp; now populated for
	                variables which have initialValue. Therefore it matches
	                the type declared in the design file. This was
	                completely unrelevant for UA Toolkit-based clients but
	                affected i.e. LabView OPC UA client.</li>
	              <li>ConfigValidator has more verbose output</li>
	              <li>When loading of configuration fails, xsd's native
	                mechanism is now used instead of running the external
	                validator (here: xmllint). This will both improve
	                portability (xmllint might be unavailable in the target
	                system), and help with configuration schemas that may
	                have very nonstandard locations.</li>
	              <li>A bug is fixed which would affect delegated
	                cachevariables of UaVariant type.<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN<br>
	          </td>
	          <td style="vertical-align: top;"> Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-156">OPCUA-156</a>]
	                - Extend config file with configuration options (e.g.
	                thread pool size)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-157">OPCUA-157</a>]
	                - Improve InformationModel for Variables</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-371">OPCUA-371</a>]
	                - LogIt: dont put full path to file which generates
	                message in the log</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-372">OPCUA-372</a>]
	                - Common way of initializing log levels at startup</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-379">OPCUA-379</a>]
	                - Merge respective changes from latest Wiener server</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-401">OPCUA-401</a>]
	                - Suboptimal error message when schema specified in
	                config.xml cannot be found</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-402">OPCUA-402</a>]
	                - DataType identifiers are necessary for certain OPC UA
	                clients</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-403">OPCUA-403</a>]
	                - UaVariant forbidden in delegated write</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-425">OPCUA-425</a>]
	                - Improvement in loading component log levels from the
	                config file</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.97<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Device classes are split into Base part (100%
	                generated) and User part (with generated stubs). This
	                helps to reduce merging efforts by an order of
	                magnitude.</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>No more compilation warnings coming from ASNodeManager</li>
	              <li>SourceVariables only rebuilt for classes which
	                actually have a source variable (and not all classes)</li>
	              <li>Configurator, Root fully autogenerated</li>
	              <li>Queriying object name is now easy in Device logic</li>
	              <li>A list of device logic files is now filled
	                automatically</li>
	              <li>Possible to have "self-referring" hasObjects relation</li>
	              <li>When given device logic body/header doesnt exist, user
	                is asked whether to generate</li>
	              <li>When given device logic body/header is older than
	                design file, user will be warned<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">
	            <ul>
	              <li>regenerating Device Logic may ask for removal of big
	                parts of already existing Device Logic code. This is
	                actually intentional as this code will now go to the
	                Base part of Device Logic class<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-259">OPCUA-259</a>]
	                - Fix build warnings</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-361">OPCUA-361</a>]
	                - ConfigValidator attempts to validate per-design
	                instantiated objects (should skip them)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-362">OPCUA-362</a>]
	                - check_consistency: add option to ask for fixing.</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-248">OPCUA-248</a>]
	                - Make SourceVariables.{h,c++} dependencies less tight</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-250">OPCUA-250</a>]
	                - Should getName() be present for all Device Logic
	                objects automatically?</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-257">OPCUA-257</a>]
	                - Evaluate Ben's anti-merge improvement</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-281">OPCUA-281</a>]
	                - Make Configurator.cpp and DRoot generation fully
	                automatic</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-349">OPCUA-349</a>]
	                - Cleanup generated_files_list and
	                generated_headers_list</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-350">OPCUA-350</a>]
	                - A warning when Device file is newer than Device Logic
	                classes</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-355">OPCUA-355</a>]
	                - Improve install tool</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-360">OPCUA-360</a>]
	                - Exchange compile-time recursion for exec-time
	                recursion of self "has" relation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-370">OPCUA-370</a>]
	                - check_consistencty: when file doesnt exist, md5sum
	                bails out</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.96<br>
	          </td>
	          <td style="vertical-align: top;"><br>
	          </td>
	          <td style="vertical-align: top;">
	            <ul>
	              <li>nullPolicy attribute needs to be added
	                per-cachevariable. This can be automated by
	                upgrade_design script (look at its help)<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-260">OPCUA-260</a>]
	                - addressSpaceWrite confused with addressSpaceRead</li>
	            </ul>
	            Improvement
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-258">OPCUA-258</a>]
	                - Better deprecation warning msg</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-263">OPCUA-263</a>]
	                - Flood of destructor messages.</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-279">OPCUA-279</a>]
	                - Focus 2nd level validation in 1 XSLT transform</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-209">OPCUA-209</a>]
	                - null setters for non-uavariant variables</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-266">OPCUA-266</a>]
	                - Failing XSLT processor should make the output to be
	                deleted</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-267">OPCUA-267</a>]
	                - Create simple XSLT for updates of the version of the
	                framework in existing projects</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.95<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Doxygen integration</li>
	              <li>Cardinality modelling (possibility to specify min and
	                max number of objects in ts relation)</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>Cachevariable's setter's source time argument (3rd
	                argument) has now a default of current datetime.
	                Therefore it can be omitted for 99% cases.<br>
	              </li>
	            </ul>
	            <br>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN<br>
	          </td>
	          <td style="vertical-align: top;"> Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-203">OPCUA-203</a>]
	                - Integrate doxygen</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-215">OPCUA-215</a>]
	                - Cardinality modelling in hasObjects relation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-231">OPCUA-231</a>]
	                - setters: current datetime (UaDateTime::now()) should
	                be default argument for time</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.94<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Config Entries</li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN<br>
	            <br>
	            Beware though that recommended layout of Device headers and
	            sources changed, and you may need to reshuffle it a bit for
	            getting easy merging.<br>
	          </td>
	          <td style="vertical-align: top;"> Bug
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-217">OPCUA-217</a>]
	                - simple addressspace setters not updated in
	                addressspace body generator</li>
	            </ul>
	            Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-168">OPCUA-168</a>]
	                - Device header and body shall have a comment that
	                indicates the start of custom section</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-197">OPCUA-197</a>]
	                - Config Entries </li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-204">OPCUA-204</a>]
	                - Document all transformations (including building the
	                project) in a nice diagram and put it into documentation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-206">OPCUA-206</a>]
	                - openssl-dev and libxml2-devel dependency missing in
	                Generic Server documentation</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-212">OPCUA-212</a>]
	                - dont generate Device Logic stubs when
	                DeviceLogicTypeName not defined</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.93<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Files database (files.txt) and number of tools that
	                depend on it: consistency checker, new installer</li>
	            </ul>
	            Minor features:<br>
	            <ul>
	              <li>Setters for simple data types take plain value instead
	                of reference</li>
	              <li>makeSetGet dropped as a cachevariable attribute
	                (equivalence of true assumed)<br>
	              </li>
	            </ul>
	            Internal features:<br>
	            <ul>
	              <li>create_release.sh script<br>
	              </li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;">NONE KNOWN <br>
	            (only deprecation warning shown for makeSetGet attribute)<br>
	          </td>
	          <td style="vertical-align: top;"> Task
	            <ul>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-120">OPCUA-120</a>]
	                - Consider makeSetGet=true as normal, non-configurable
	                parameter</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-127">OPCUA-127</a>]
	                - A way to check that files that shall not be modified
	                (transforms...?) are not modified</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-160">OPCUA-160</a>]
	                - Install script should be more gentle ...</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-167">OPCUA-167</a>]
	                - Setters: simple data types (integers, floats...) shall
	                be passed by value rather than by reference</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-188">OPCUA-188</a>]
	                - A script to verify if proper things are committed to
	                the SVN (and warn if too much/not enough is within
	                version control)</li>
	              <li>[<a href="https://its.cern.ch/jira/browse/OPCUA-198">OPCUA-198</a>]
	                - Create script for tagging Generic Server releases</li>
	            </ul>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	        <tr>
	          <td style="vertical-align: top;">0.92<br>
	          </td>
	          <td style="vertical-align: top;">Major features:<br>
	            <ul>
	              <li>Source variables</li>
	              <li>Automated mutex generation<br>
	              </li>
	              <li>UaVariant added as allowed data type</li>
	              <li>honkyTonk tool to simulate address space updates<br>
	              </li>
	            </ul>
	            Improvements:<br>
	            <ul>
	              <li>Unified and cleaned initialization of cache variables</li>
	              <li>Configuration.xsd: proper types for various flavours
	                of integers (previously every int irrespectively of
	                length and signedness was just "int")</li>
	              <li>createDiagram.sh creates diagrams of varied level of
	                details</li>
	            </ul>
	          </td>
	          <td style="vertical-align: top;"><br>
	          </td>
	          <td style="vertical-align: top;"><br>
	          </td>
	          <td valign="top">(potential remarks were sent in the release
	            email)</td>
	        </tr>
	      </tbody>
	    </table>
