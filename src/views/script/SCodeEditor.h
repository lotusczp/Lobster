/****************************************************************************
**
** Copyright (C) 2016 Ken Crossen, bugs corrected, code cleaned up
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Redistributions in source code or binary form may not be sold.
**
****************************************************************************/

/*
  This file is part of the Ofi Labs X2 project.

  Copyright (C) 2011 Ariya Hidayat <ariya.hidayat@gmail.com>
  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SCODEEDITOR_H
#define SCODEEDITOR_H

#include <QtCore>
#include <QtGui>

#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QCompleter>
#include <QColor>

class SCodeEditor;
class JSDocLayout;
class SidebarWidget;
class JSHighlighter;

// class JSEditPrivate;
class JSEditPrivate {
public:
    SCodeEditor *editor;
    JSDocLayout *layout;
    JSHighlighter *highlighter;
    SidebarWidget *sidebar;
    bool showLineNumbers;
    bool textWrap;
    QColor cursorColor;
    bool bracketsMatching;
    QList<int> matchPositions;
    QColor bracketMatchColor;
    QList<int> errorPositions;
    QColor bracketErrorColor;
    bool codeFolding : 1;

    bool Brace_Bracket_Character;
    bool Quote_Bracket_Character;
    bool Post_Select_Bracket_Enclosed_Text;

    bool AutoIndentEnabled;
    int Tab_Modulus;

    QRegularExpression JavaScript_Bracket_RegEx;
    QString Bracket_Source_Text;
    QString Bracket_Text;

    // If this is to be a distributed library, new data members ...
    // ... should be added at the end of this "structure" (class).
};

class SCodeEditor: public QPlainTextEdit
{
    Q_OBJECT
    Q_PROPERTY(bool bracketsMatchingEnabled READ isBracketsMatchingEnabled WRITE setBracketsMatchingEnabled)
    Q_PROPERTY(bool codeFoldingEnabled READ isCodeFoldingEnabled WRITE setCodeFoldingEnabled)
    Q_PROPERTY(bool lineNumbersVisible READ isLineNumbersVisible WRITE setLineNumbersVisible)
    Q_PROPERTY(bool textWrapEnabled READ isTextWrapEnabled WRITE setTextWrapEnabled)
    Q_PROPERTY(bool AutoIndentEnabled READ isAutoIndentEnabled WRITE setAutoIndentEnabled)

public:
    typedef enum {
        Background,
        Normal,
        Comment,
        Number,
        String,
        Operator,
        Identifier,
        Keyword,
        BuiltIn,
        Sidebar,
        LineNumber,
        Cursor,
        Marker,
        BracketMatch,
        BracketError,
        FoldIndicator
    } ColorComponent;

    SCodeEditor ( QWidget *parent = 0 );
    ~SCodeEditor ( );

    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;


#ifdef use_d_pointer
#else
    JSEditPrivate JSEdit_Private;
#endif

    void
    Set_PlainText ( QString Set_Plain_Text );

    void
    setColor ( ColorComponent component,
               const QColor &color );

    QStringList keywords ( ) const;

    void
    setKeywords ( const QStringList &keywords );

public slots:
    void
    setBracketsMatchingEnabled ( bool enable );

public:
    bool
    isBracketsMatchingEnabled ( ) const;

public slots:
    void
    setLineNumbersVisible ( bool visible );

public:
    bool
    isLineNumbersVisible ( ) const;

public slots:
    void
    setTextWrapEnabled ( bool enable );

public:
    bool
    isTextWrapEnabled ( ) const;

public slots:
    void
    setAutoIndentEnabled ( bool enable );

public:
    bool
    isAutoIndentEnabled ( ) const;

public:
    void
    Set_Tab_Modulus ( int New_Tab_modulus );

public slots:
    void
    setCodeFoldingEnabled ( bool enable );

public:
    bool
    isCodeFoldingEnabled ( ) const;

    bool
    isFoldable ( int line );

    bool
    isFolded ( int line ) const;

public slots:
    void
    fold ( int line );

    void
    unfold ( int line );

    void
    toggleFold ( int line );

public slots:
    void
    mark ( const QString &str,
           Qt::CaseSensitivity sens = Qt::CaseInsensitive );

protected:
    void
    keyPressEvent ( QKeyEvent* event );

    void
    resizeEvent ( QResizeEvent* event );

    void
    wheelEvent ( QWheelEvent* event );

    void
    insertFromMimeData ( const QMimeData* source );

private:
#define JavaScript_Bracket_List "{}()"
#define Default_Tab_Modulus 4

    QString
    Compute_Bracket_Text ( QString Source_Text );

    int
    Bracket_Match_Position ( int Current_Position );

    int
    Compute_Current_Brace_Indent_Level ( int Current_Position );

    int
    Compute_Current_Brace_Indent ( int Current_Position );

    int
    Compute_Current_Paren_Indent ( QString Current_Text,
                                   int Current_Position );

    QTextCursor
    Select_Selected_Text_Lines ( );

    void
    Format_Selected_Text_Lines ( );

    void
    Indent_Selected_Text_Lines ( int Indent_Tab_Modulus );

private slots:
    void
    onTextChanged ( );

private:
//    int
//    findClosingMatch ( const QTextDocument *doc, int cursorPosition );

    int
    findClosingConstruct ( const QTextBlock &block );

private slots:
    void
    updateCursor ( );

public slots:
    void
    updateSidebar ( );

private slots:
    void
    updateSidebar ( const QRect &rect,
                    int d );

    /// this is for completer
    void insertCompletion(const QString &completion);


private:
#ifdef use_d_pointer
    QScopedPointer<JSEditPrivate> d_ptr;
#endif

    QCompleter *m_pCompleter;

    //Q_DECLARE_PRIVATE(SCodeEditor);
    Q_DISABLE_COPY(SCodeEditor);
};

#endif // SCODEEDITOR_H
