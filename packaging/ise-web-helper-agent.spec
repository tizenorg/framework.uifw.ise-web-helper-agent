Name:       ise-web-helper-agent
Summary:    Tizen web keyboard container
Version:    0.0.1
Release:    1
Group:      System Environment/Libraries
License:    Apache License, Version 2.0
Source0:    ise-web-helper-agent-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  gettext-tools
BuildRequires:  cmake
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(isf)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(ewebkit2)
BuildRequires:  pkgconfig(libwebsockets)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(wrt-core)
BuildRequires:  pkgconfig(dpl-efl)
BuildRequires:  capi-appfw-package-manager-devel


%description
Description: Web keyboard container


%prep
%setup -q

%build
export CFLAGS+=" -DTIZEN_ENGINEER_MODE"
export CXXFLAGS+=" -DTIZEN_ENGINEER_MODE"
export FFLAGS+=" -DTIZEN_ENGINEER_MODE"

%if "%{?tizen_profile_name}" == "mobile"
CFLAGS+=" -D_MOBILE";
CXXFLAGS+=" -D_MOBILE";
%elseif "%{?tizen_profile_name}" == "wearable"
CFLAGS+=" -D_WEARABLE";
CXXFLAGS+=" -D_WEARABLE";
%endif

rm -rf CMakeFiles
rm -rf CMakeCache.txt
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%make_install

%post

%postun

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/scim-1.0/1.4.0/Helper/ise-web-helper-agent.so
%{_libdir}/scim-1.0/web-container.so
%{_datadir}/packages/*
/usr/share/license/%{name}
/etc/smack/accesses.d/%{name}.efl
