/*
    KiiroNotes - simple notes for Linux desktop
    Copyright (C) 2016  Błażej Szczygieł

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QSystemTrayIcon>
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

#include "Notes.hpp"

#include <signal.h>

static Notes *notes;

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ForceRasterWidgets);
    QApplication app(argc, argv);

    /**/

    QTranslator qtTranslator, translator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    translator.load(":/" + QLocale::system().name());
    QCoreApplication::installTranslator(&qtTranslator);
    QCoreApplication::installTranslator(&translator);

    /**/

    app.setApplicationVersion("v0.0.2");
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName("KiiroNotes");
    app.setWindowIcon(QIcon::fromTheme("KiiroNotes"));

    /**/

    notes = new Notes;

    /**/

    QSystemTrayIcon tray;
    tray.setToolTip(app.applicationDisplayName());
    tray.setContextMenu(notes->getMenu());
    tray.setIcon(app.windowIcon());
    tray.show();

    /**/

    QObject::connect(&tray, &QSystemTrayIcon::activated, [](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            notes->toggleVisibility();
    });

    /**/

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [] {
        QMetaObject::invokeMethod(notes, "save");
    });

    signal(SIGTERM, [](int) {
        QMetaObject::invokeMethod(notes, "save");
    });

    /**/

    return app.exec();
}
