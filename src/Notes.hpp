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

#ifndef NOTES_HPP
#define NOTES_HPP

#include <QWidget>
#include <QSettings>

class QPlainTextEdit;
class QTimer;
class QMenu;

class Notes : public QWidget
{
    Q_OBJECT

public:
    Notes();

    inline QMenu *getMenu() const
    {
        return m_menu;
    }

    void toggleVisibility();

    Q_SLOT void save();

private:
    inline Qt::WindowFlags getNotesWindowFlags() const;

    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void closeEvent(QCloseEvent *event) final;

    const QString m_notesFileName;
    QSettings m_settings;
    QPlainTextEdit *m_textEdit;
    QMenu *m_menu;
    QTimer *m_saveTimer;

    QAction *m_visibilityAct;

    QPoint m_mouseOrigin;
    bool m_canMove;

    bool m_visible;
};

#endif // NOTES_HPP
