# quasar
Quick opcUA Server generAtion fRamework

## Out-of-the-box dependencies.
Once you've cloned the quasar framework git repository to your development machine you'll need to obtain some dependencies before you can start developing. Due to licensing, these dependencies can't be distributed with the quasar code, but the following table describes what is required and how to get it.

### On linux
|Dependency Name|Why is it needed?|Where do I get it?|Where do I put it?|
|-------------- |---------------- |----------------- |----------------- |
|Unified Automation C++ SDK |Provides the UA stack and higher level functions |<www.unified-automation.com> At time of writing you can try before you buy, see the evaluation download. quasar also works with the evaluation version (subject to the vendor's evaluation constraints |Follow the installation instructions from the vendor; the SDK needs to be installed on the development machine|
|ServerConfig.xml |Configures the UA stack (provided by the Unified Automation SDK) |From the install location of the Unified Automation SDK, locate it with grep and/or find |Copy it to _quasar-checkout-dir_/bin |
|saxon9he.jar   |XSLT processor, used to generate implementation code from quasar's abstract code templates and your OPC-UA server design|Download the jar file from the saxon sourceforge page <http://sourceforge.net/projects/saxon>. At time of writing `wget http://sourceforge.net/projects/saxon/files/latest/download?source=files`|Copy the jar file to _quasar-checkout-dir_/Design/saxon9he.jar |

### On windows
To be added, thanks Damian.
