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

#include "Notes.hpp"

#include <QStandardPaths>
#include <QPlainTextEdit>
#include <QApplication>
#include <QMessageBox>
#include <QGridLayout>
#include <QSaveFile>
#include <QSizeGrip>
#include <QSettings>
#include <qevent.h>
#include <QAction>
#include <QTimer>
#include <QFile>
#include <QMenu>

Notes::Notes() :
    m_notesFileName(QStandardPaths::standardLocations(QStandardPaths::DataLocation).value(0)),
    m_settings(QApplication::applicationName()),
    m_textEdit(new QPlainTextEdit),
    m_menu(new QMenu(this)),
    m_saveTimer(new QTimer(this)),

    m_canMove(false)
{
    QAction *saveAct = new QAction(tr("Save"), this);
    saveAct->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveAct, &QAction::triggered, this, &Notes::save);

    m_visibilityAct = new QAction(tr("Show"), this);
    m_visibilityAct->setProperty("SecondText", tr("Hide"));
    connect(m_visibilityAct, &QAction::triggered, this, &Notes::toggleVisibility);

    QAction *stayOnBottomAct = new QAction(tr("Stay on bottom"), this);
    stayOnBottomAct->setShortcut(QKeySequence("Alt+B"));
    stayOnBottomAct->setCheckable(true);
    connect(stayOnBottomAct, &QAction::triggered, [this, stayOnBottomAct] {
        Qt::WindowFlags flags = getNotesWindowFlags();
        if (stayOnBottomAct->isChecked())
            flags |= Qt::WindowStaysOnBottomHint;
        setWindowFlags(flags);
        setVisible(m_visible);
    });

    QAction *aboutAct = new QAction(tr("About"), this);
    aboutAct->setShortcut(QKeySequence("F1"));
    connect(aboutAct, &QAction::triggered, [this] {
        QMessageBox::information(this, QString(), tr("KiiroNotes - simple notes for Linux desktop") + " (" + QCoreApplication::applicationVersion() + ")");
    });

    QAction *closeAct = new QAction(tr("Close"), this);
    closeAct->setShortcut(QKeySequence("Ctrl+Q"));
    connect(closeAct, &QAction::triggered, this, &Notes::close);

    /**/

    m_saveTimer->setSingleShot(true);
    connect(m_saveTimer, &QTimer::timeout, this, &Notes::save);

    /**/

    m_menu->addAction(saveAct);
    m_menu->addSeparator();
    m_menu->addAction(m_visibilityAct);
    m_menu->addAction(stayOnBottomAct);
    m_menu->addSeparator();
    m_menu->addAction(aboutAct);
    m_menu->addSeparator();
    m_menu->addAction(closeAct);

    /**/

    m_textEdit->setFrameShadow(QFrame::Plain);
    m_textEdit->setFrameShape(QFrame::NoFrame);

    QFile file(m_notesFileName);
    if (file.open(QFile::ReadOnly))
    {
        m_textEdit->setPlainText(file.readAll());
        m_textEdit->moveCursor(QTextCursor::End);
        m_textEdit->document()->setModified(false);
    }

    connect(m_textEdit, &QPlainTextEdit::textChanged, [this] {
        m_saveTimer->start(10000);
    });

    /**/

    QPalette palette;
    palette.setBrush(QPalette::Window, QColor(0xF7DC9D));
    palette.setBrush(QPalette::Base, QColor(0xF7EC9D));

    setPalette(palette);

    addAction(saveAct);
    addAction(m_visibilityAct);
    addAction(stayOnBottomAct);
    addAction(closeAct);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(m_textEdit, 0, 0, 1, 2);
    gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 0, 1, 1);
    gridLayout->addWidget(new QSizeGrip(this), 1, 1, 1, 1);
    gridLayout->setMargin(0);

    /**/

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &Notes::customContextMenuRequested, [this](const QPoint &p) {
        m_menu->popup(mapToGlobal(p));
    });

    Qt::WindowFlags flags = getNotesWindowFlags();
    if (m_settings.value("StayOnBottom", false).toBool())
    {
        flags |= Qt::WindowStaysOnBottomHint;
        stayOnBottomAct->setChecked(true);
    }
    setWindowFlags(flags);

    if (!restoreGeometry(m_settings.value("Geometry").toByteArray()))
        resize(300, 400);

    if (m_settings.value("Visible", true).toBool())
        toggleVisibility();
    else
        m_visible = false;
}

void Notes::toggleVisibility()
{
    if (isVisible())
    {
        hide();
        m_visibilityAct->setShortcut(QKeySequence());
        m_visible = false;
    }
    else
    {
        show();
        m_visibilityAct->setShortcut(QKeySequence("Escape"));
        m_visible = true;
    }

    const QString tmpText = m_visibilityAct->text();
    m_visibilityAct->setText(m_visibilityAct->property("SecondText").toString());
    m_visibilityAct->setProperty("SecondText", tmpText);
}

void Notes::save()
{
    const bool fromTimerOrAction = (sender() == m_saveTimer || qobject_cast<QAction *>(sender()));
    if (!fromTimerOrAction || m_textEdit->document()->isModified())
    {
        QSaveFile file(m_notesFileName);
        if (file.open(QSaveFile::WriteOnly))
        {
            file.write(m_textEdit->toPlainText().toUtf8());
            if (file.commit())
                m_textEdit->document()->setModified(false);
        }
    }
    if (!fromTimerOrAction)
    {
        m_settings.setValue("Geometry", saveGeometry());
        m_settings.setValue("Visible", m_visible);
        m_settings.setValue("StayOnBottom", !!(windowFlags() & Qt::WindowStaysOnBottomHint));
        m_settings.sync();
    }
}

inline Qt::WindowFlags Notes::getNotesWindowFlags() const
{
    return Qt::Tool | Qt::FramelessWindowHint;
}

void Notes::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_mouseOrigin = event->pos();
        setCursor(Qt::SizeAllCursor);
        m_canMove = true;
    }
    QWidget::mousePressEvent(event);
}
void Notes::mouseMoveEvent(QMouseEvent *event)
{
    if (m_canMove)
        move(mapToGlobal(event->pos() - m_mouseOrigin));
    QWidget::mouseMoveEvent(event);
}
void Notes::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setCursor(QCursor());
        m_canMove = false;
    }
    QWidget::mouseReleaseEvent(event);
}
void Notes::closeEvent(QCloseEvent *event)
{
    if (sender())
    {
        if (QMessageBox::question(this, QString(), tr("Do you want to close?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            event->ignore();
            return;
        }
        QCoreApplication::quit();
        deleteLater();
    }
    QWidget::closeEvent(event);
}
