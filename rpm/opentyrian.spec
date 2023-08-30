Name:       opentyrian
Summary:    opentyrian / opentyrian
Release:    1
Version:    2.1
Group:      opentyrian
License:    GPLv2
URL:        https://github.com/opentyrian/opentyrian
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  cmake
BuildRequires:  SDL2-devel
%description
OpenTyrian is an open-source port of the DOS game Tyrian.

Tyrian is an arcade-style vertical scrolling shooter.  The story is set
in 20,031 where you play as Trent Hawkins, a skilled fighter-pilot employed
to fight MicroSol and save the galaxy.

%prep
%setup -q

%build
export LANG=en_US.UTF-8
%cmake
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_datadir}/%{name}/data
#cp -r data %{buildroot}%{_datadir}/%{name}/data
install %{name} %{buildroot}%{_bindir}/%{name}
install -D data/* -t %{buildroot}%{_datadir}/%{name}/data

%files
%{_bindir}/%{name}
%{_datadir}/%{name}/data

%changelog

