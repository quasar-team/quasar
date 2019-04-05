%define name __SET_NAME_OF_YOUR_PACKAGE__ 
%define _topdir %(echo $PWD)
%define _tmpdir %{_topdir}/tmp
%define PREFIX /opt/%{name}

AutoReqProv: yes 
Summary: %{name}
Name: %{name}
Version: %{version}
Release: %{release}
Source0: checkout.tar.gz
License: zlib/libpng license
Group: Development/Application
BuildRoot: %{_topdir}/BUILDROOT/%{name}-%{version}
BuildArch: x86_64
Prefix: %{PREFIX}
Vendor: CERN 
# you can put Requires: line here if there are dependencies which can't be automatically resolved by RPM

%description
This is our super-amazing-fancy OPC UA server for INSERT NAME OF EQUIPMENT HERE.
Based on Generic OPC UA Server framework by ATLAS Central DCS, CERN.
Responsible: ... Put developers name and email address here

%prep
echo ">>> setup tag" 
echo %{name}

%setup -n checkout 

%build
echo "--- Build ---"
python ./quasar.py set_build_config config_cc7_toolkit155.cmake || exit 1
python ./quasar.py build Release || exit 1
python ./quasar.py generate as_doc || exit 1
python ./quasar.py generate config_doc || exit 1

%install
echo "--- Install (don't confuse with installation; nothing is installed on your system now in fact...) ---"
INSTALLED_DIR=%{buildroot}/%{PREFIX}/bin
INSTALLED_DOC_DIR=%{buildroot}/%{PREFIX}/doc
/bin/mkdir -p $INSTALLED_DIR 
/bin/mkdir -p $INSTALLED_DOC_DIR
/bin/cp -v \
    build/bin/OpcUaServer \
    bin/ServerConfig.xml \   
    $INSTALLED_DIR  # <-- destination

/bin/cp -v \
        Documentation/AddressSpaceDoc.html \
	Documentation/ConfigDocumentation.html \
	$INSTALLED_DOC_DIR  # <-- destination

# here you give a list of other files to install

%pre
echo "Pre-install: nothing to do"

%post
echo "Generating OPC UA Server Certificate..."
%{PREFIX}/bin/OpcUaServer --create_certificate


%preun

if [ $1 = 0 ]; then
	echo "Pre-uninstall: Complete uninstall: will remove files"
fi

%postun
if [ $1 = 0 ]; then
    echo "Post-uninstall: Complete uninstall: will remove files"
fi

# 
# Hint: if your server installs any shared objects, you should run ldconfig here. 
#

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root)
%{PREFIX}
# additional paths ...

%changelog
