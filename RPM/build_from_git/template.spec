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
Requires: OpcUaToolkit

%description
This is our super-amazing-fancy OPC UA server for INSERT NAME OF EQUIPMENT HERE.
Based on Generic OPC UA Server framework by ATLAS Central DCS, CERN.

%prep
echo ">>> setup tag" 
echo %{name}

%setup -n checkout 

%build
echo "--- Build ---"
export UNIFIED_AUTOMATION_OPCUA_SDK=/opt/OpcUaToolkit-1.3.3
sh ./automated_build_linux.sh

%install
echo "--- Install (don't confuse with installation; nothing is installed on your system now in fact...) ---"
INSTALLED_DIR=%{buildroot}/%{PREFIX}/bin
/bin/mkdir -p $INSTALLED_DIR 
/bin/cp bin/Server                                         $INSTALLED_DIR
/bin/cp bin/ServerConfig.xml                         $INSTALLED_DIR/ServerConfig.xml
/bin/cp Configuration/Configuration.xsd                    $INSTALLED_DIR 

%pre
echo "Pre-install: nothing to do"

%post
echo "Generating OPC UA Server Certificate..."
%{PREFIX}/bin/Server --create_certificate


%preun


# 
# Hint: if your server installs any shared objects, you should run ldconfig here. 
#

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root)
%{PREFIX}

%changelog
