# KiiroNotes
Simple notes for Linux desktop

### Requires
- C++11 compiler,
- Qt5.

### Arch Linux AUR package
```sh
yaourt -S kiiro-notes-git
```

### Compilation and installation
```sh
cd src
lrelease-qt5 KiiroNotes.pro
qmake-qt5
make -j2
sudo install -D -m 755 -s KiiroNotes /usr/bin/KiiroNotes
sudo cp -r ../share /usr
sudo update-desktop-database
sudo gtk-update-icon-cache /usr/share/icons/hicolor
```

### Autostart
```sh
cp /usr/share/applications/KiiroNotes.desktop ~/.config/autostart
```
