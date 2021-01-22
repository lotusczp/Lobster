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

// For explaination, see "D-Pointer" on Qt Wiki. Cliff's Notes version follows:
// To compile JSEdit as a distributed binary library that can be upgraded w/o ...
// ... breaking code designed for old binary when using a newer upgraded binary, ...
// ... i.e., not compiling JSEdit source code directly into executable.
// For binary library distribution, uncomment line below to #define use_d_pointer.
// #define use_d_pointer
// Otherwise, JSEditPrivate should be compiled from the source code and ...
// ... static linked into the executable as a class (essentially a pretentious STRUCT).
// For static linking into executable, comment line above to #undef use_d_pointer.

#ifdef use_d_pointer
    // For distribution as binary library
    #define JSEditPrivate_ref(JSEdit_Private_Class_Member) d_ptr->JSEdit_Private_Class_Member
#else
    // For static linking into executable
    #define JSEditPrivate_ref(JSEdit_Private_Class_Member) JSEdit_Private.JSEdit_Private_Class_Member
#endif

#include "SCodeEditor.h"

#include <QApplication>
#include <QtGui>

QString Test_JS_RegEx =
R"~~~(

function strip() {
  return this.replace(/^\s+/, '').replace(/\s+$/, '');
}

function stripTags() {
  return this.replace(/<\w+(\s+("[^"]*"|'[^']*'|[^>])+)?>|<\/\w+>/gi, '');
}

function stripScripts() {
  return this.replace(new RegExp(Prototype.ScriptFragment, 'img'), '');
}

/=(?:[^\[|]*?(?:\[\[[^|\]]*(?:\|(?:[^|\]]*))?\]\])?)+(?:\||\}\})/

)~~~";

class JSBlockData: public QTextBlockUserData
{
public:
    QList<int> bracketPositions;
};

class JSHighlighter : public QSyntaxHighlighter {
public:
    JSHighlighter ( QTextDocument *parent = 0 );
    void setColor(SCodeEditor::ColorComponent component, const QColor &color);
    void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);

    QStringList keywords() const;
    void setKeywords(const QStringList &keywords);

protected:
    void highlightBlock(const QString &text);

private:
    QSet<QString> m_keywords;
    QSet<QString> m_knownIds;
    QHash<SCodeEditor::ColorComponent, QColor> m_colors;
    QString m_markString;
    Qt::CaseSensitivity m_markCaseSensitivity;
};

JSHighlighter::JSHighlighter ( QTextDocument *parent ) : QSyntaxHighlighter ( parent ),
                                                         m_markCaseSensitivity(Qt::CaseInsensitive) {
    // default color scheme
    m_colors[SCodeEditor::Normal]     = QColor(0, 0, 0); // QColor("#000000");
    m_colors[SCodeEditor::Comment]    = QColor(128, 128, 128); // QColor("#808080");
    m_colors[SCodeEditor::Number]     = QColor(192, 0, 0); // QColor("#008000");
    m_colors[SCodeEditor::String]     = QColor(0, 128, 0); // QColor("#800000");
    m_colors[SCodeEditor::Operator]   = QColor(0, 0, 0); // QColor("#808000");
    m_colors[SCodeEditor::Identifier] = QColor(128, 0, 128); // QColor("#000020");
    m_colors[SCodeEditor::Keyword]    = QColor(0, 0, 192); // QColor("#000080");
    m_colors[SCodeEditor::BuiltIn]    = QColor(128, 0, 192); // QColor("#008080");
    m_colors[SCodeEditor::Marker]     = QColor(255, 255, 0); // QColor("#ffff00");

    // https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
    m_keywords << "break";
    m_keywords << "case";
    m_keywords << "catch";
    m_keywords << "continue";
    m_keywords << "default";
    m_keywords << "delete";
    m_keywords << "do";
    m_keywords << "else";
    m_keywords << "finally";
    m_keywords << "for";
    m_keywords << "function";
    m_keywords << "if";
    m_keywords << "in";
    m_keywords << "instanceof";
    m_keywords << "new";
    m_keywords << "return";
    m_keywords << "switch";
    m_keywords << "this";
    m_keywords << "throw";
    m_keywords << "try";
    m_keywords << "typeof";
    m_keywords << "var";
    m_keywords << "void";
    m_keywords << "while";
    m_keywords << "with";

    m_keywords << "true";
    m_keywords << "false";
    m_keywords << "null";

    // built-in and other popular objects + properties
    m_knownIds << "Object";
    m_knownIds << "prototype";
    m_knownIds << "create";
    m_knownIds << "defineProperty";
    m_knownIds << "defineProperties";
    m_knownIds << "getOwnPropertyDescriptor";
    m_knownIds << "keys";
    m_knownIds << "getOwnPropertyNames";
    m_knownIds << "constructor";
    m_knownIds << "__parent__";
    m_knownIds << "__proto__";
    m_knownIds << "__defineGetter__";
    m_knownIds << "__defineSetter__";
    m_knownIds << "eval";
    m_knownIds << "hasOwnProperty";
    m_knownIds << "isPrototypeOf";
    m_knownIds << "__lookupGetter__";
    m_knownIds << "__lookupSetter__";
    m_knownIds << "__noSuchMethod__";
    m_knownIds << "propertyIsEnumerable";
    m_knownIds << "toSource";
    m_knownIds << "toLocaleString";
    m_knownIds << "toString";
    m_knownIds << "unwatch";
    m_knownIds << "valueOf";
    m_knownIds << "watch";

    m_knownIds << "Function";
    m_knownIds << "arguments";
    m_knownIds << "arity";
    m_knownIds << "caller";
    m_knownIds << "constructor";
    m_knownIds << "length";
    m_knownIds << "name";
    m_knownIds << "apply";
    m_knownIds << "bind";
    m_knownIds << "call";

    m_knownIds << "String";
    m_knownIds << "fromCharCode";
    m_knownIds << "length";
    m_knownIds << "charAt";
    m_knownIds << "charCodeAt";
    m_knownIds << "concat";
    m_knownIds << "indexOf";
    m_knownIds << "lastIndexOf";
    m_knownIds << "localCompare";
    m_knownIds << "match";
    m_knownIds << "quote";
    m_knownIds << "replace";
    m_knownIds << "search";
    m_knownIds << "slice";
    m_knownIds << "split";
    m_knownIds << "substr";
    m_knownIds << "substring";
    m_knownIds << "toLocaleLowerCase";
    m_knownIds << "toLocaleUpperCase";
    m_knownIds << "toLowerCase";
    m_knownIds << "toUpperCase";
    m_knownIds << "trim";
    m_knownIds << "trimLeft";
    m_knownIds << "trimRight";

    m_knownIds << "Array";
    m_knownIds << "isArray";
    m_knownIds << "index";
    m_knownIds << "input";
    m_knownIds << "pop";
    m_knownIds << "push";
    m_knownIds << "reverse";
    m_knownIds << "shift";
    m_knownIds << "sort";
    m_knownIds << "splice";
    m_knownIds << "unshift";
    m_knownIds << "concat";
    m_knownIds << "join";
    m_knownIds << "filter";
    m_knownIds << "forEach";
    m_knownIds << "every";
    m_knownIds << "map";
    m_knownIds << "some";
    m_knownIds << "reduce";
    m_knownIds << "reduceRight";

    m_knownIds << "RegExp";
    m_knownIds << "global";
    m_knownIds << "ignoreCase";
    m_knownIds << "lastIndex";
    m_knownIds << "multiline";
    m_knownIds << "source";
    m_knownIds << "exec";
    m_knownIds << "test";

    m_knownIds << "JSON";
    m_knownIds << "parse";
    m_knownIds << "stringify";

    m_knownIds << "decodeURI";
    m_knownIds << "decodeURIComponent";
    m_knownIds << "encodeURI";
    m_knownIds << "encodeURIComponent";
    m_knownIds << "eval";
    m_knownIds << "isFinite";
    m_knownIds << "isNaN";
    m_knownIds << "parseFloat";
    m_knownIds << "parseInt";
    m_knownIds << "Infinity";
    m_knownIds << "NaN";
    m_knownIds << "undefined";

    m_knownIds << "Math";
    m_knownIds << "E";
    m_knownIds << "LN2";
    m_knownIds << "LN10";
    m_knownIds << "LOG2E";
    m_knownIds << "LOG10E";
    m_knownIds << "PI";
    m_knownIds << "SQRT1_2";
    m_knownIds << "SQRT2";
    m_knownIds << "abs";
    m_knownIds << "acos";
    m_knownIds << "asin";
    m_knownIds << "atan";
    m_knownIds << "atan2";
    m_knownIds << "ceil";
    m_knownIds << "cos";
    m_knownIds << "exp";
    m_knownIds << "floor";
    m_knownIds << "log";
    m_knownIds << "max";
    m_knownIds << "min";
    m_knownIds << "pow";
    m_knownIds << "random";
    m_knownIds << "round";
    m_knownIds << "sin";
    m_knownIds << "sqrt";
    m_knownIds << "tan";

    m_knownIds << "document";
    m_knownIds << "window";
    m_knownIds << "navigator";
    m_knownIds << "userAgent";
}

void
JSHighlighter::setColor ( SCodeEditor::ColorComponent component, const QColor &color ) {
    m_colors[component] = color;
    rehighlight();
}

void
JSHighlighter::highlightBlock ( const QString &text ) {
    // parsing state
    enum {
        Start = 0,
        Number = 1,
        Identifier = 2,
        String = 3,
        Comment = 4,
        Regex = 5
    };

    QList<int> bracketPositions;

    int blockState = previousBlockState();
    int bracketLevel = blockState >> 4;
    int state = blockState & 15;
    if (blockState < 0) {
        bracketLevel = 0;
        state = Start;
    }

    int start = 0;
    int i = 0;
    while (i <= text.length()) {
        QChar ch = (i < text.length()) ? text.at(i) : QChar();
        QChar next_ch = (i < text.length() - 1) ? text.at(i + 1) : QChar();

        switch (state) {

        case Start:
            start = i;
            if (ch.isSpace()) {
                ++i;
            } else if (ch.isDigit()) {
                ++i;
                state = Number;
            } else if (ch.isLetter() || (ch == '_') || (ch == '$')) {
                ++i;
                state = Identifier;
            } else if ((ch == '\'') || (ch == '\"')) {
                ++i;
                state = String;
            } else if ((ch == '/') && (next_ch == '*')) {
                ++i;
                ++i;
                state = Comment;
            } else if ((ch == '/') && (next_ch == '/')) {
                i = text.length();
                setFormat(start, text.length(), m_colors[SCodeEditor::Comment]);
            } else if ((ch == '/') && (next_ch != '*')) {
                ++i;
                state = Regex;
            } else {
                if (! QString("(){}[]").contains(ch))
                    setFormat(start, 1, m_colors[SCodeEditor::Operator]);
                if ((ch =='{') || (ch == '}')) {
                    bracketPositions += i;
                    if (ch == '{')
                        bracketLevel++;
                    else
                        bracketLevel--;
                }
                ++i;
                state = Start;
            }
            break;

        case Number:
            if (ch.isSpace() || (! ch.isDigit())) {
                setFormat(start, i - start, m_colors[SCodeEditor::Number]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case Identifier:
            if (ch.isSpace() ||
                (! (ch.isDigit() || ch.isLetter() || (ch == '_') || (ch == '$')))) {
                QString token = text.mid(start, i - start).trimmed();
                if (m_keywords.contains(token))
                    setFormat(start, i - start, m_colors[SCodeEditor::Keyword]);
                else if (m_knownIds.contains(token))
                    setFormat(start, i - start, m_colors[SCodeEditor::BuiltIn]);
                else
                    setFormat(start, i - start, m_colors[SCodeEditor::Identifier]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case String:
            if ((ch == '\\') &&
                ((next_ch == '\\') || (next_ch == '\'') || (next_ch == '\"') ||
                 (next_ch == 'b') || (next_ch == 'r') || (next_ch == 'f') ||
                 (next_ch == 't') || (next_ch == 'v'))) {
                // Accept all valid escapes as part of string
                ++i;
                ++i;
            }
            else if (ch == text.at(start)) {
                QChar prev_prev = (i > 1) ? text.at(i - 2) : QChar();
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if ((! (prev == '\\')) || ((prev_prev == '\\') && (prev == '\\'))) {
                    ++i;
                    setFormat(start, i - start, m_colors[SCodeEditor::String]);
                    state = Start;
                }
                else {
                    // If (ch == '\\') and we are here, there's an error.
                    // For example, an invalid escape sequence.
                    ++i;
                }
            } else {
                // If (ch == '\\') and we are here, there's an error
                // For example, an invalid escape sequence.
                ++i;
            }
            break;

        case Comment:
            if ((ch == '*') && (next_ch == '/')) {
                ++i;
                ++i;
                setFormat(start, i - start, m_colors[SCodeEditor::Comment]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case Regex:
            if (ch == '\\') {
                // Accept all escapes as part of regex
                ++i;
                ++i;
            }
            else if (ch == '/') {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (! (prev == '\\')) {
                    // Get the modifiers also
                    while (QString("igm").contains(next_ch)) {
                        ++i;
                        next_ch = (i < text.length() - 1) ? text.at(i + 1) : QChar();
                    }
                    ++i;
                    setFormat(start, i - start, m_colors[SCodeEditor::String]);
                    state = Start;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;

        default:
            state = Start;
            break;
        }
    }

    if (state == Comment)
        setFormat(start, text.length(), m_colors[SCodeEditor::Comment]);
    else
        state = Start;

    if (!m_markString.isEmpty()) {
        int pos = 0;
        int len = m_markString.length();
        QTextCharFormat markerFormat;
        markerFormat.setBackground(m_colors[SCodeEditor::Marker]);
        markerFormat.setForeground(m_colors[SCodeEditor::Normal]);
        for (;;) {
            pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
            if (pos < 0)
                break;
            setFormat(pos, len, markerFormat);
            ++pos;
        }
    }

    if (!bracketPositions.isEmpty()) {
        JSBlockData *blockData = reinterpret_cast<JSBlockData*>(currentBlock().userData());
        if (!blockData) {
            blockData = new JSBlockData;
            currentBlock().setUserData(blockData);
        }
        blockData->bracketPositions = bracketPositions;
    }

    blockState = (state & 15) | (bracketLevel << 4);
    setCurrentBlockState(blockState);
}

void
JSHighlighter::mark ( const QString &str, Qt::CaseSensitivity caseSensitivity ) {
    m_markString = str;
    m_markCaseSensitivity = caseSensitivity;
    rehighlight();
}

QStringList
JSHighlighter::keywords ( ) const {
    return m_keywords.toList();
}

void
JSHighlighter::setKeywords ( const QStringList &keywords ) {
    m_keywords = QSet<QString>::fromList(keywords);
    rehighlight();
}

struct BlockInfo {
    int position;
    int number;
    bool foldable: 1;
    bool folded : 1;
};

Q_DECLARE_TYPEINFO(BlockInfo, Q_PRIMITIVE_TYPE);

class SidebarWidget : public QWidget {
public:
    SidebarWidget(SCodeEditor *editor);
    QVector<BlockInfo> lineNumbers;
    QColor backgroundColor;
    QColor lineNumberColor;
    QColor indicatorColor;
    QColor foldIndicatorColor;
    QFont font;
    int foldIndicatorWidth;
    QPixmap rightArrowIcon;
    QPixmap downArrowIcon;
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
};

SidebarWidget::SidebarWidget ( SCodeEditor *editor ) : QWidget ( editor ),
                                                  foldIndicatorWidth ( 0 ) {
    backgroundColor = QColor(200, 200, 200);
    lineNumberColor = Qt::black;
    indicatorColor = Qt::white;
    foldIndicatorColor = Qt::lightGray;
}

void
SidebarWidget::mousePressEvent ( QMouseEvent *event ) {
    if (foldIndicatorWidth > 0) {
        int xofs = width() - foldIndicatorWidth;
        int lineNo = -1;
        int fh = fontMetrics().lineSpacing();
        int ys = event->pos().y();
        if (event->pos().x() > xofs) {
            foreach (BlockInfo ln, lineNumbers)
                if (ln.position < ys && (ln.position + fh) > ys) {
                    if (ln.foldable)
                        lineNo = ln.number;
                    break;
                }
        }
        if (lineNo >= 0) {
            SCodeEditor *editor = qobject_cast<SCodeEditor*>(parent());
            if (editor)
                editor->toggleFold(lineNo);
        }
    }
}

void
SidebarWidget::paintEvent ( QPaintEvent *event ) {
    QPainter p(this);
    p.fillRect(event->rect(), backgroundColor);
    p.setPen(lineNumberColor);
    p.setFont(font);
    int fh = QFontMetrics(font).height();
    foreach (BlockInfo ln, lineNumbers)
        p.drawText(0, ln.position, width() - 4 - foldIndicatorWidth, fh, Qt::AlignRight, QString::number(ln.number));

    if (foldIndicatorWidth > 0) {
        int xofs = width() - foldIndicatorWidth;
        p.fillRect(xofs, 0, foldIndicatorWidth, height(), indicatorColor);

        // initialize (or recreate) the arrow icons whenever necessary
        if (foldIndicatorWidth != rightArrowIcon.width()) {
            QPainter iconPainter;
            QPolygonF polygon;

            int dim = foldIndicatorWidth;
            rightArrowIcon = QPixmap(dim, dim);
            rightArrowIcon.fill(Qt::transparent);
            downArrowIcon = rightArrowIcon;

            polygon << QPointF(dim * 0.4, dim * 0.25);
            polygon << QPointF(dim * 0.4, dim * 0.75);
            polygon << QPointF(dim * 0.8, dim * 0.5);
            iconPainter.begin(&rightArrowIcon);
            iconPainter.setRenderHint(QPainter::Antialiasing);
            iconPainter.setPen(Qt::NoPen);
            iconPainter.setBrush(foldIndicatorColor);
            iconPainter.drawPolygon(polygon);
            iconPainter.end();

            polygon.clear();
            polygon << QPointF(dim * 0.25, dim * 0.4);
            polygon << QPointF(dim * 0.75, dim * 0.4);
            polygon << QPointF(dim * 0.5, dim * 0.8);
            iconPainter.begin(&downArrowIcon);
            iconPainter.setRenderHint(QPainter::Antialiasing);
            iconPainter.setPen(Qt::NoPen);
            iconPainter.setBrush(foldIndicatorColor);
            iconPainter.drawPolygon(polygon);
            iconPainter.end();
        }

        foreach (BlockInfo ln, lineNumbers)
            if (ln.foldable) {
                if (ln.folded)
                    p.drawPixmap(xofs, ln.position, rightArrowIcon);
                else
                    p.drawPixmap(xofs, ln.position, downArrowIcon);
            }
    }
}

class JSDocLayout: public QPlainTextDocumentLayout {
public:
    JSDocLayout ( QTextDocument *doc );
    void forceUpdate();
};

JSDocLayout::JSDocLayout ( QTextDocument *doc ) : QPlainTextDocumentLayout ( doc ) {
}

void
JSDocLayout::forceUpdate ( ) {
    emit documentSizeChanged(documentSize());
}

#ifdef use_d_pointer
JSEdit::JSEdit ( QWidget *parent ) : QPlainTextEdit( parent ),
                                     d_ptr ( new JSEditPrivate ) {
#else
SCodeEditor::SCodeEditor ( QWidget *parent ) : QPlainTextEdit( parent ) {
#endif
    JSEditPrivate_ref(editor) = this;
    JSEditPrivate_ref(layout) = new JSDocLayout(document());
    JSEditPrivate_ref(highlighter) = new JSHighlighter(document());
    JSEditPrivate_ref(sidebar) = new SidebarWidget(this);
    JSEditPrivate_ref(showLineNumbers) = true;
    JSEditPrivate_ref(textWrap) = true;
    JSEditPrivate_ref(bracketsMatching) = true;
    JSEditPrivate_ref(cursorColor) = QColor(255, 255, 192);
    JSEditPrivate_ref(bracketMatchColor) = QColor(128, 255, 128);
    JSEditPrivate_ref(bracketErrorColor) = QColor(255, 128, 128);
    JSEditPrivate_ref(codeFolding) = true;

    JSEditPrivate_ref(AutoIndentEnabled) = true;
    JSEditPrivate_ref(Tab_Modulus) = Default_Tab_Modulus;

    JSEditPrivate_ref(Brace_Bracket_Character) = true;
    JSEditPrivate_ref(Quote_Bracket_Character) = true;
    JSEditPrivate_ref(Post_Select_Bracket_Enclosed_Text) = true;

    QStringList expand_keywords = this->keywords();
    expand_keywords << "const" << "let";
    this->setKeywords(expand_keywords);

    // this->setColor(JSEdit::Background,    QColor(255, 255, 255)); // QColor("#0C152B"));
    // this->setColor(JSEdit::Normal,        QColor(0, 0, 0)); // QColor("#FFFFFF"));
    // this->setColor(JSEdit::Comment,       QColor(128, 128, 128)); // QColor("#666666"));
    // this->setColor(JSEdit::Number,        QColor(192, 0, 0)); // QColor("#DBF76C"));
    // this->setColor(JSEdit::String,        QColor(0, 128, 0)); // QColor("#5ED363"));
    // this->setColor(JSEdit::Operator,      QColor(0, 0, 0)); // QColor("#FF7729"));
    this->setColor(SCodeEditor::Identifier,    QColor(128, 0, 128)); // QColor("#FFFFFF"));
    this->setColor(SCodeEditor::Keyword,       QColor(0, 160, 160)); // QColor("#FDE15D"));
    this->setColor(SCodeEditor::BuiltIn,       QColor(0, 128, 192)); // QColor("#9CB6D4"));
    // this->setColor(JSEdit::Cursor,        QColor(255, 255, 192)); // QColor("#1E346B"));
    // this->setColor(JSEdit::Marker,        QColor(255, 255, 0)); // QColor("#DBF76C"));
    // this->setColor(JSEdit::BracketMatch,  QColor(128, 255, 128)); // QColor("#1AB0A6"));
    // this->setColor(JSEdit::BracketError,  QColor(255, 128, 128)); // QColor("#A82224"));
    // this->setColor(JSEdit::FoldIndicator, Qt::lightGray); // QColor("#555555"));

    document()->setDocumentLayout(JSEditPrivate_ref(layout));

    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursor()));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSidebar()));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateSidebar(QRect, int)));

    QString doublequoted_string = "\"(?:[^\\\\\"]|\\\\.)*\"";
    QString singlequoted_string = "'(?:[^\\\\']|\\\\.)*'";
    QString c_style_comment = "/\\*(?:[^*]*|\\*[^/])*\\*/";
    QString doubleslash_comment = "//[^\\n]*\\n";
    // js regex literal must come after comment /* and comment //
    QString js_regex_literal = "(/(?:\\\\.|[^/])+/[igm]*)";
    QString bracket_characters = "[{}()]";

    JSEditPrivate_ref(JavaScript_Bracket_RegEx) =
      QRegularExpression(doublequoted_string + "|" + singlequoted_string + "|" +
                         c_style_comment + "|" + doubleslash_comment + "|" +
                         js_regex_literal + "|" + bracket_characters);

    QString js_identifier = "([A-Za-z$_][A-Za-z0-9$_]*)";
    js_identifier.clear(); // Don't use

    m_pCompleter = nullptr;

#if defined(Q_OS_MAC)
    QFont textFont = font();
    textFont.setPointSize(15);
    textFont.setFamily("Courier");
    setFont(textFont);
#elif defined(Q_OS_UNIX)
    QFont textFont = font();
    textFont.setFamily("Monospace");
    setFont(textFont);
#endif
}

SCodeEditor::~SCodeEditor ( ) {
    delete JSEditPrivate_ref(layout);
}

void SCodeEditor::setCompleter(QCompleter *completer)
{
    if (m_pCompleter)
        QObject::disconnect(m_pCompleter, 0, this, 0);

    m_pCompleter = completer;

    if (!m_pCompleter)
        return;

    m_pCompleter->setWidget(this);
    m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
    m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(m_pCompleter, SIGNAL(activated(QString)),
                  this, SLOT(insertCompletion(QString)));
}

QCompleter *SCodeEditor::completer() const
{
    return m_pCompleter;
}

void
SCodeEditor::Set_PlainText ( QString Set_Plain_Text ) {
    // Don't defeat Undo/Redo
    QPlainTextEdit::selectAll();
    QPlainTextEdit::insertPlainText(Set_Plain_Text);
    QTextCursor txt_cursor = QPlainTextEdit::textCursor();
    txt_cursor.movePosition(QTextCursor::Start);
    QPlainTextEdit::setTextCursor(txt_cursor);
}

QString
SCodeEditor::Compute_Bracket_Text ( QString Source_Text ) {
    QString bracket_list = JavaScript_Bracket_List;
    QString bracket_text = QString(" ").repeated(Source_Text.length());
    if (JSEditPrivate_ref(JavaScript_Bracket_RegEx).isValid()) {
        QRegularExpressionMatchIterator regex_iterator =
                                          JSEditPrivate_ref(JavaScript_Bracket_RegEx.globalMatch(Source_Text));
        if (regex_iterator.hasNext()) {
            // At least one found
            while (regex_iterator.hasNext()) {
                QRegularExpressionMatch match = regex_iterator.next();
                if ((match.capturedLength() == 1) &&
                    bracket_list.contains(match.captured())) {
                    bracket_text[match.capturedStart()] = match.captured().at(0);
                }
            }
        }
    }
    return bracket_text;
}

int
SCodeEditor::Bracket_Match_Position ( int Current_Position ) {
    QString target_text = this->toPlainText();

    if (! (target_text.length() == JSEditPrivate_ref(Bracket_Text).length())) onTextChanged();
    if (! (target_text == JSEditPrivate_ref(Bracket_Source_Text))) onTextChanged();

    QString bracket_list = JavaScript_Bracket_List;

    if ((Current_Position < 0) || (Current_Position >= target_text.length()) ||
        (Current_Position >= JSEditPrivate_ref(Bracket_Text).length())) return -1;
    if (! bracket_list.contains(target_text.at(Current_Position))) return -1;

    // For example, may have found bracket_list character in comment or string.
    if (! (JSEditPrivate_ref(Bracket_Text).at(Current_Position) == target_text.at(Current_Position))) return -1;

    int match_position = Current_Position;
    int paren_level = 0;
    if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '(') {
        while (match_position < JSEditPrivate_ref(Bracket_Text).length()) {
            if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '(') paren_level += 1;
            else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == ')') paren_level -= 1;
            if (paren_level == 0) break;
            match_position += 1;
        }
    }
    else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == ')') {
        while (match_position >= 0) {
            if (JSEditPrivate_ref(Bracket_Text).at(match_position) == ')') paren_level += 1;
            else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '(') paren_level -= 1;
            if (paren_level == 0) break;
            match_position -= 1;
        }
    }
    else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '[') {
        while (match_position < JSEditPrivate_ref(Bracket_Text).length()) {
            if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '[') paren_level += 1;
            else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == ']') paren_level -= 1;
            if (paren_level == 0) break;
            match_position += 1;
        }
    }
    else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == ']') {
        while (match_position >= 0) {
            if (JSEditPrivate_ref(Bracket_Text).at(match_position) == ']') paren_level += 1;
            else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '[') paren_level -= 1;
            if (paren_level == 0) break;
            match_position -= 1;
        }
    }
    else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '{') {
        while (match_position < JSEditPrivate_ref(Bracket_Text).length()) {
            if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '{') paren_level += 1;
            else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '}') paren_level -= 1;
            if (paren_level == 0) break;
            match_position += 1;
        }
    }
    else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '}') {
        while (match_position >= 0) {
            if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '}') paren_level += 1;
            else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '{') paren_level -= 1;
            if (paren_level == 0) break;
            match_position -= 1;
        }
    }

    if ((match_position >= 0) && (match_position < JSEditPrivate_ref(Bracket_Text).length())) {
        return match_position;
    }

    return -1; // No match found
}

int
SCodeEditor::Compute_Current_Brace_Indent_Level ( int Current_Position ) {
    // Returns brace_indent_level in units of Tab_Modulus
    int match_position = Current_Position;
    int brace_indent_level = 0;
    while (match_position >= 0) {
        if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '}') brace_indent_level -= 1;
        else if (JSEditPrivate_ref(Bracket_Text).at(match_position) == '{') brace_indent_level += 1;
        match_position -= 1;
    }
    return brace_indent_level;
}

int
SCodeEditor::Compute_Current_Brace_Indent ( int Current_Position ) {
    // Returns brace_indent in units of spaces
    int brace_indent_level = Compute_Current_Brace_Indent_Level(Current_Position);
    return (brace_indent_level * JSEditPrivate_ref(Tab_Modulus));
}

int
SCodeEditor::Compute_Current_Paren_Indent ( QString Current_Text,
                                       int Current_Position ) {
    // Returns paren_indent in units of spaces
    QString bracket_text = Compute_Bracket_Text(Current_Text);
    // For example:
    // if (((cell_x + 1) == parseInt(x_and_y[0])) && <Return Here>
    // Or for  example:
    // if (((cell_x + 1) == parseInt(x_and_y[0])) &&
    //     ((cell_y + 1) == parseInt(x_and_y[1]))) cells += "X";
    int paren_indent = 0;
    // Search back for closest unmatched "(", how is it "indented"?
    int paren_level = 0;
    int paren_position = Current_Position;
    while (paren_position > 0) {
        if (bracket_text.at(paren_position) == QChar('{')) break;
        else if (bracket_text.at(paren_position) == QChar(')')) paren_level -= 1;
        else if (bracket_text.at(paren_position) == QChar('(')) {
            paren_level += 1;
            if (paren_level == 1) {
                // Found closest unmatched "(", now find immediately preceding newline
                int newline_position = paren_position;
                while ((newline_position > 0) &&
                       (! (Current_Text.at(newline_position) == QChar('\n')))) newline_position -= 1;
                // Found immediately preceding newline
                if ((newline_position >= 0) &&
                    (Current_Text.at(newline_position) == QChar('\n'))) {
                    paren_indent = paren_position - newline_position;
                    break;
                }
            }
        }
        paren_position -= 1;
    }

    return paren_indent;
}

void
SCodeEditor::keyPressEvent ( QKeyEvent* event ) {
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

    if (((event->key() == Qt::Key_ParenLeft) ||
         (event->key() == Qt::Key_BracketLeft) ||
         ((event->key() == Qt::Key_BraceLeft) && JSEditPrivate_ref(Brace_Bracket_Character)) ||
         ((event->key() == Qt::Key_QuoteDbl) && JSEditPrivate_ref(Quote_Bracket_Character)) ||
         ((event->key() == Qt::Key_Apostrophe) && JSEditPrivate_ref(Quote_Bracket_Character))) &&
        ((modifiers & Qt::ControlModifier) == Qt::NoModifier) &&
        (this->textCursor().selectedText().count() > 0)) {
        event->accept();
        // For these "bracketing" characters, if the "opening" character is typed ...
        // ... when text is selected, the selected text will be enclosed by ...
        // ... "open" and "close" characters.
        QString left_encloser;
        QString right_encloser;
        if (event->key() == Qt::Key_ParenLeft) {
            left_encloser = "(";
            right_encloser = ")";
        }
        else if (event->key() == Qt::Key_BracketLeft) {
            left_encloser = "[";
            right_encloser = "]";
        }
        else if (event->key() == Qt::Key_BraceLeft) {
            left_encloser = "{";
            right_encloser = "}";
        }
        else if (event->key() == Qt::Key_QuoteDbl) {
            left_encloser = "\"";
            right_encloser = "\"";
        }
        else if (event->key() == Qt::Key_Apostrophe) {
            left_encloser = "'";
            right_encloser = "'";
        }

        QTextCursor txt_cursor = this->textCursor();
        int sel_begin_pos = txt_cursor.selectionStart();
        int sel_end_pos = txt_cursor.selectionEnd();
        txt_cursor.insertText(left_encloser + txt_cursor.selectedText() + right_encloser);

        if (JSEditPrivate_ref(Post_Select_Bracket_Enclosed_Text)) {
            txt_cursor.setPosition((sel_begin_pos + 1));
            txt_cursor.setPosition((sel_end_pos + 1), QTextCursor::KeepAnchor);
            this->setTextCursor(txt_cursor);
        }

        return;
    }
    else if (event->key() == Qt::Key_BraceLeft) {
        // Insert formatted brace pair, ready for 'braced' code
        int initial_position = QPlainTextEdit::textCursor().position();
        QPlainTextEdit::insertPlainText("{\n}");
        QTextCursor txt_cursor = QPlainTextEdit::textCursor();

        txt_cursor.setPosition(initial_position, QTextCursor::KeepAnchor);
        QPlainTextEdit::setTextCursor(txt_cursor);
        // Use general formatting code
        Format_Selected_Text_Lines();

        // Put cursor right after the 'keyboarded' left brace, as expected
        txt_cursor.setPosition(initial_position + 1);
        QPlainTextEdit::setTextCursor(txt_cursor);
    }
    else if ((event->key() == Qt::Key_Return) && JSEditPrivate_ref(AutoIndentEnabled)) {
        QTextCursor txt_cursor = QPlainTextEdit::textCursor();
        QString before_cursor_text = QPlainTextEdit::toPlainText();
        before_cursor_text.truncate(txt_cursor.position());
        int indent_levels = Compute_Current_Brace_Indent_Level(txt_cursor.position());

        // For example:
        // if (((cell_x + 1) == parseInt(x_and_y[0])) && <Return Here>
        int paren_indent = Compute_Current_Paren_Indent(before_cursor_text,
                                                        (before_cursor_text.length() - 1));

        QString whitespace = QString(" ").repeated(indent_levels * JSEditPrivate_ref(Tab_Modulus));
        // paren_indent in units of spaces, indent_levels in units of Tab_Modulus
        paren_indent = paren_indent - (indent_levels * JSEditPrivate_ref(Tab_Modulus));
        if (paren_indent > 0) whitespace += QString(" ").repeated(paren_indent);

        QPlainTextEdit::insertPlainText("\n" + whitespace);
    }
    else if (event->key() == Qt::Key_Tab) {
        QTextCursor txt_cursor = QPlainTextEdit::textCursor();
        if (txt_cursor.selectedText().length() == 0) {
            txt_cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString line_text_before_cursor = txt_cursor.selectedText();
            if (line_text_before_cursor.indexOf(QRegExp("[^\\s]")) < 0) {
                // Only whitespace before cursor
                txt_cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                QString line_text_after_cursor = txt_cursor.selectedText();
                int first_non_whitespace_idx = line_text_after_cursor.indexOf(QRegExp("[^\\s]"));
                QString whitespace = line_text_before_cursor;
                if (first_non_whitespace_idx > 0) {
                    QTextCursor new_txt_cursor = QPlainTextEdit::textCursor();
                    new_txt_cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, first_non_whitespace_idx);
                    // Move cursor to just before first non-whitespace character
                    QPlainTextEdit::setTextCursor(new_txt_cursor);
                    line_text_after_cursor.truncate(first_non_whitespace_idx);
                    whitespace += line_text_after_cursor;
                }
                // Add enough whitespace to bring first non-whitespace character to just after next tab modulus
                QPlainTextEdit::insertPlainText(QString(" ").repeated(JSEditPrivate_ref(Tab_Modulus) - (whitespace.length() % JSEditPrivate_ref(Tab_Modulus))));
            }
            else {
                // Non-whitespace before cursor, just add tab modulus spaces
                QPlainTextEdit::insertPlainText(QString(" ").repeated(JSEditPrivate_ref(Tab_Modulus)));
            }
        }
        else if ((modifiers & Qt::AltModifier) == Qt::NoModifier) {
            Indent_Selected_Text_Lines(JSEditPrivate_ref(Tab_Modulus));
        }
    }
    else if ((event->key() == Qt::Key_BracketLeft) &&
             ((modifiers & Qt::ControlModifier) == Qt::ControlModifier) &&
             ((modifiers & Qt::ShiftModifier) == Qt::NoModifier)) {
        Indent_Selected_Text_Lines(-2);
    }
    else if ((event->key() == Qt::Key_BracketRight) &&
             ((modifiers & Qt::ControlModifier) == Qt::ControlModifier) &&
             ((modifiers & Qt::ShiftModifier) == Qt::NoModifier)) {
        Indent_Selected_Text_Lines(2);
    }
    else if ((event->key() == Qt::Key_BracketLeft) &&
             ((modifiers & Qt::ControlModifier) == Qt::ControlModifier) &&
             ((modifiers & Qt::ShiftModifier) == Qt::ShiftModifier)) {
        Indent_Selected_Text_Lines(-1);
    }
    else if ((event->key() == Qt::Key_BracketRight) &&
             ((modifiers & Qt::ControlModifier) == Qt::ControlModifier) &&
             ((modifiers & Qt::ShiftModifier) == Qt::ShiftModifier)) {
        Indent_Selected_Text_Lines(1);
    }
    else if ((event->key() == Qt::Key_Equal) &&
             ((modifiers & Qt::ControlModifier) == Qt::ControlModifier) &&
             ((modifiers & Qt::ShiftModifier) == Qt::NoModifier)) {
        Format_Selected_Text_Lines();
    }
    else {
        // Allow parent class (normal) handling of key
        QPlainTextEdit::keyPressEvent(event);
    }
}

QTextCursor
SCodeEditor::Select_Selected_Text_Lines ( ) {
    QTextCursor begin_txt_cursor = QPlainTextEdit::textCursor();
    begin_txt_cursor.setPosition(begin_txt_cursor.selectionStart());
    begin_txt_cursor.select(QTextCursor::LineUnderCursor);

    QTextCursor end_txt_cursor = QPlainTextEdit::textCursor();
    end_txt_cursor.setPosition(end_txt_cursor.selectionEnd());
    end_txt_cursor.select(QTextCursor::LineUnderCursor);

    QTextCursor new_txt_cursor = QPlainTextEdit::textCursor();
    new_txt_cursor.setPosition(qMin(qMin(begin_txt_cursor.selectionStart(),
                                         begin_txt_cursor.selectionEnd()),
                                    qMin(end_txt_cursor.selectionStart(),
                                         end_txt_cursor.selectionEnd())));
    new_txt_cursor.setPosition(qMax(qMax(begin_txt_cursor.selectionStart(),
                                         begin_txt_cursor.selectionEnd()),
                                    qMax(end_txt_cursor.selectionStart(),
                                         end_txt_cursor.selectionEnd())),
                               QTextCursor::KeepAnchor);
    QPlainTextEdit::setTextCursor(new_txt_cursor);

    return new_txt_cursor;
}

void
SCodeEditor::Format_Selected_Text_Lines ( ) {
    QTextCursor new_txt_cursor = Select_Selected_Text_Lines();
    QString before_selected_text = QPlainTextEdit::toPlainText();
    before_selected_text.truncate(new_txt_cursor.selectionStart());
    QString selected_text = new_txt_cursor.selectedText();
    selected_text = selected_text.replace(QChar(0x2029), QChar('\n'));
    QString bracket_text = Compute_Bracket_Text(selected_text);
    // Selection (when extended to include all of any line w/ any text selected), ...
    // ... together with the text preceding the current selection, ...
    // ... must be syntactically "complete" in the upward direction, ...
    // ... i.e. no unmatched "}" or ")" but unmatched "{" or "(" are accepted.
    // In other words, "…{…{…}…}…" and "…{…{…}…" are fine but "…{…}…}…" is not.

    int position = bracket_text.length() - 1;
    while (position >= 0) {
        if (bracket_text.at(position) == '{') {
            int insert_position = position + 1;
            while (insert_position < selected_text.length()) {
                QChar ch = selected_text.at(insert_position);
                if (ch.isSpace() && (! (ch == QChar('\n')))) insert_position += 1;
                else break;
            }
            if (insert_position < selected_text.length()) {
                if (! (selected_text.at(insert_position) == QChar('\n')))
                    selected_text.insert(insert_position, "\n");
            }
        }
        else if (bracket_text.at(position) == '}') {
            int insert_position = position + 1;
            while (insert_position < selected_text.length()) {
                QChar ch = selected_text.at(insert_position);
                if (ch.isSpace() && (! (ch == QChar('\n')))) insert_position += 1;
                else break;
            }
            if (insert_position < selected_text.length()) {
                if (! (selected_text.at(insert_position) == QChar('\n')))
                    selected_text.insert(insert_position, "\n");
            }
        }
        position -= 1;
    }

    int current_indent_level = Compute_Current_Brace_Indent_Level(new_txt_cursor.selectionStart());
    // Indenting whitespace computed for position at beginning of selection (extended).
    // QString whitespace = QString(" ").repeated(indent_levels * Tab_Modulus);
    // Split into individual lines that can be indented independently as syntax requires
    QStringList selected_lines = selected_text.split("\n");
    QString indented_text = "";
    int bracket_level = 0;
    for (int line_idx = 0; line_idx < selected_lines.count(); line_idx += 1) {
        QString line_text = selected_lines[line_idx];
        // Remove all whitespace and replace with computed whitespace
        line_text = line_text.trimmed();
        int bracket_indent_level = bracket_level;

        // For example:
        // if (((cell_x + 1) == parseInt(x_and_y[0])) &&
        //     ((cell_y + 1) == parseInt(x_and_y[1]))) cells += "X";
        QString before_current_text = before_selected_text + indented_text;
        int paren_indent = Compute_Current_Paren_Indent(before_current_text,
                                                        (before_current_text.length() - 1));

        if ((line_text.startsWith('}')) && (line_text.endsWith('}'))) bracket_indent_level -= 1;
        indented_text += QString(" ").repeated((current_indent_level + bracket_indent_level) * JSEditPrivate_ref(Tab_Modulus));
        // paren_indent is count of individual speces, while *_indent_level are in units of Tab_Modulus
        paren_indent = paren_indent - ((current_indent_level + bracket_indent_level) * JSEditPrivate_ref(Tab_Modulus));
        indented_text += QString(" ").repeated(paren_indent);
        indented_text += line_text;
        if (line_idx < (selected_lines.count() - 1)) indented_text += "\n";
        if (line_text.endsWith('{')) bracket_level += 1;
        else if (line_text.endsWith('}')) bracket_level -= 1;
    }
    QPlainTextEdit::insertPlainText(indented_text);
}

void
SCodeEditor::Indent_Selected_Text_Lines ( int Indent_Tab_Modulus ) {
    if (Indent_Tab_Modulus == 0) return;

    QTextCursor new_txt_cursor = Select_Selected_Text_Lines();
    int start_position = new_txt_cursor.selectionStart();
    QString selected_text = new_txt_cursor.selectedText();
    QStringList selected_text_lines = selected_text.split(QChar(0x2029));
    QString indented_text = "";
    if (Indent_Tab_Modulus > 0) {
        for (int line_idx = 0; line_idx < selected_text_lines.count(); line_idx += 1) {
            indented_text += QString(" ").repeated(Indent_Tab_Modulus) + selected_text_lines.at(line_idx);
            if (line_idx < (selected_text_lines.count() - 1)) indented_text += "\n";
        }
    }
    else if (Indent_Tab_Modulus < 0) {
        for (int line_idx = 0; line_idx < selected_text_lines.count(); line_idx += 1) {
            QString line_text = selected_text_lines.at(line_idx);
            if (line_text.startsWith(QString(" ").repeated(-Indent_Tab_Modulus))) {
                line_text = line_text.mid(-Indent_Tab_Modulus);
            }
            indented_text += line_text;
            if (line_idx < (selected_text_lines.count() - 1)) indented_text += "\n";
        }
    }
    QPlainTextEdit::insertPlainText(indented_text);
    new_txt_cursor.setPosition(start_position, QTextCursor::KeepAnchor);
    QPlainTextEdit::setTextCursor(new_txt_cursor);
}

void
SCodeEditor::insertFromMimeData ( const QMimeData* source ) {
    if (source->hasText()) {
        QString paste_text = source->text();
        // Convert tab characters into tab modulus spaces
        paste_text = paste_text.replace("\t", QString(" ").repeated(JSEditPrivate_ref(Tab_Modulus)));
        QStringList paste_lines = paste_text.split("\n");
        if (paste_lines.count() == 1) {
            QPlainTextEdit::insertPlainText(paste_text);
        }
        else {
            // Indent pasted text intelligently ...
            int initial_position = QPlainTextEdit::textCursor().position();
            QPlainTextEdit::insertPlainText(paste_text);
            QTextCursor txt_cursor = QPlainTextEdit::textCursor();

            txt_cursor.setPosition(initial_position, QTextCursor::KeepAnchor);
            QPlainTextEdit::setTextCursor(txt_cursor);
            // ... using general formatting code
            Format_Selected_Text_Lines();
        }
    }
}

void
SCodeEditor::onTextChanged ( ) {
    JSEditPrivate_ref(Bracket_Source_Text) = this->toPlainText();
    JSEditPrivate_ref(Bracket_Text) = Compute_Bracket_Text(JSEditPrivate_ref(Bracket_Source_Text));
}

void
SCodeEditor::setColor ( ColorComponent component, const QColor &color ) {
    if (component == Background) {
        QPalette pal = palette();
        pal.setColor(QPalette::Base, color);
        setPalette(pal);
        JSEditPrivate_ref(sidebar)->indicatorColor = color;
        updateSidebar();
    } else if (component == Normal) {
        QPalette pal = palette();
        pal.setColor(QPalette::Text, color);
        setPalette(pal);
    } else if (component == Sidebar) {
        JSEditPrivate_ref(sidebar)->backgroundColor = color;
        updateSidebar();
    } else if (component == LineNumber) {
        JSEditPrivate_ref(sidebar)->lineNumberColor = color;
        updateSidebar();
    } else if (component == Cursor) {
        JSEditPrivate_ref(cursorColor) = color;
        updateCursor();
    } else if (component == BracketMatch) {
        JSEditPrivate_ref(bracketMatchColor) = color;
        updateCursor();
    } else if (component == BracketError) {
        JSEditPrivate_ref(bracketErrorColor) = color;
        updateCursor();
    } else if (component == FoldIndicator) {
        JSEditPrivate_ref(sidebar)->foldIndicatorColor = color;
        updateSidebar();
    } else {
        JSEditPrivate_ref(highlighter)->setColor(component, color);
        updateCursor();
    }
}

void
SCodeEditor::setKeywords ( const QStringList &keywords ) {
    JSEditPrivate_ref(highlighter)->setKeywords(keywords);
}

QStringList
SCodeEditor::keywords ( ) const {
    return JSEditPrivate_ref(highlighter)->keywords();
}

void
SCodeEditor::setLineNumbersVisible ( bool visible ) {
    JSEditPrivate_ref(showLineNumbers) = visible;
    updateSidebar();
}

bool
SCodeEditor::isLineNumbersVisible ( ) const {
    return JSEditPrivate_ref(showLineNumbers);
}

void
SCodeEditor::setTextWrapEnabled ( bool enable ) {
    JSEditPrivate_ref(textWrap) = enable;
    setLineWrapMode(enable ? WidgetWidth : NoWrap);
}

bool
SCodeEditor::isTextWrapEnabled ( ) const {
    return JSEditPrivate_ref(textWrap);
}

void
SCodeEditor::setBracketsMatchingEnabled ( bool enable ) {
    JSEditPrivate_ref(bracketsMatching) = enable;
    updateCursor();
}

bool
SCodeEditor::isBracketsMatchingEnabled ( ) const {
    return JSEditPrivate_ref(bracketsMatching);
}

void
SCodeEditor::setAutoIndentEnabled ( bool enable ) {
    JSEditPrivate_ref(AutoIndentEnabled) = enable;
}

bool
SCodeEditor::isAutoIndentEnabled ( ) const {
    return JSEditPrivate_ref(AutoIndentEnabled);
}

void
SCodeEditor::Set_Tab_Modulus ( int New_Tab_Modulus ) {
    JSEditPrivate_ref(Tab_Modulus) = New_Tab_Modulus;
}

void
SCodeEditor::setCodeFoldingEnabled ( bool enable ) {
    JSEditPrivate_ref(codeFolding) = enable;
    updateSidebar();
}

bool
SCodeEditor::isCodeFoldingEnabled ( ) const {
    return JSEditPrivate_ref(codeFolding);
}

bool
SCodeEditor::isFoldable ( int line ) {
    int matchPos = findClosingConstruct(document()->findBlockByNumber(line - 1));
    if (matchPos >= 0) {
        QTextBlock matchBlock = document()->findBlock(matchPos);
        if (matchBlock.isValid() && matchBlock.blockNumber() > line)
            return true;
    }
    return false;
}

bool
SCodeEditor::isFolded ( int line ) const {
    QTextBlock block = document()->findBlockByNumber(line - 1);
    if (!block.isValid())
        return false;
    block = block.next();
    if (!block.isValid())
        return false;
    return !block.isVisible();
}

void
SCodeEditor::fold ( int line ) {
    QTextBlock startBlock = document()->findBlockByNumber(line - 1);
    int endPos = findClosingConstruct(startBlock);
    if (endPos < 0)
        return;
    QTextBlock endBlock = document()->findBlock(endPos);

    QTextBlock block = startBlock.next();
    while (block.isValid() && block != endBlock) {
        block.setVisible(false);
        block.setLineCount(0);
        block = block.next();
    }

    document()->markContentsDirty(startBlock.position(), endPos - startBlock.position() + 1);
    updateSidebar();
    update();

    JSDocLayout *layout = reinterpret_cast<JSDocLayout*>(document()->documentLayout());
    layout->forceUpdate();
}

void
SCodeEditor::unfold ( int line ) {
    QTextBlock startBlock = document()->findBlockByNumber(line - 1);
    int endPos = findClosingConstruct(startBlock);

    QTextBlock block = startBlock.next();
    while (block.isValid() && !block.isVisible()) {
        block.setVisible(true);
        block.setLineCount(block.layout()->lineCount());
        endPos = block.position() + block.length();
        block = block.next();
    }

    document()->markContentsDirty(startBlock.position(), endPos - startBlock.position() + 1);
    updateSidebar();
    update();

    JSDocLayout *layout = reinterpret_cast<JSDocLayout*>(document()->documentLayout());
    layout->forceUpdate();
}

void
SCodeEditor::toggleFold ( int line ) {
    if (isFolded(line))
        unfold(line);
    else
        fold(line);
}

int
SCodeEditor::findClosingConstruct ( const QTextBlock &block ) {
    if (!block.isValid())
        return -1;
    JSBlockData *blockData = reinterpret_cast<JSBlockData*>(block.userData());
    if (!blockData)
        return -1;
    if (blockData->bracketPositions.isEmpty())
        return -1;
    const QTextDocument *doc = block.document();
    int offset = block.position();
    foreach (int pos, blockData->bracketPositions) {
        int absPos = offset + pos;
        if (doc->characterAt(absPos) == '{') {
            int matchPos = Bracket_Match_Position(absPos); // findClosingMatch(doc, absPos);
            if (matchPos >= 0)
                return matchPos;
        }
    }
    return -1;
}

void
SCodeEditor::resizeEvent ( QResizeEvent *event ) {
    QPlainTextEdit::resizeEvent(event);
    updateSidebar();
}

void
SCodeEditor::wheelEvent ( QWheelEvent *event ) {
    if (event->modifiers() == Qt::ControlModifier) {
        int steps = event->delta() / 20;
        steps = qBound(-3, steps, 3);
        QFont textFont = font();
        int pointSize = textFont.pointSize() + steps;
        pointSize = qBound(10, pointSize, 40);
        textFont.setPointSize(pointSize);
        setFont(textFont);
        updateSidebar();
        event->accept();
        return;
    }
    QPlainTextEdit::wheelEvent(event);
}


void
SCodeEditor::updateCursor ( ) {
    if (isReadOnly()) {
        setExtraSelections(QList<QTextEdit::ExtraSelection>());
    } else {

        JSEditPrivate_ref(matchPositions).clear();
        JSEditPrivate_ref(errorPositions).clear();

        if (JSEditPrivate_ref(bracketsMatching) /* && textCursor().block().userData() */ ) {
            QTextCursor cursor = textCursor();
            int cursorPosition = cursor.position();

            if ((document()->characterAt(cursorPosition) == '{') ||
                (document()->characterAt(cursorPosition) == '(')) {
                int matchPos = Bracket_Match_Position(cursorPosition); // findClosingMatch(document(), cursorPosition);
                if (matchPos < 0) {
                    JSEditPrivate_ref(errorPositions) += cursorPosition;
                } else {
                    JSEditPrivate_ref(matchPositions) += cursorPosition;
                    JSEditPrivate_ref(matchPositions) += matchPos;
                }
            }

            if ((document()->characterAt(cursorPosition - 1) == '}') ||
                (document()->characterAt(cursorPosition - 1) == ')')) {
                int matchPos = Bracket_Match_Position(cursorPosition - 1); // findOpeningMatch(document(), cursorPosition);
                if (matchPos < 0) {
                    JSEditPrivate_ref(errorPositions) += cursorPosition - 1;
                } else {
                    JSEditPrivate_ref(matchPositions) += cursorPosition - 1;
                    JSEditPrivate_ref(matchPositions) += matchPos;
                }
            }
        }

        QTextEdit::ExtraSelection highlight;
        highlight.format.setBackground(JSEditPrivate_ref(cursorColor));
        highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
        highlight.cursor = textCursor();
        highlight.cursor.clearSelection();

        QList<QTextEdit::ExtraSelection> extraSelections;
        extraSelections.append(highlight);

        for (int i = 0; i < JSEditPrivate_ref(matchPositions).count(); ++i) {
            int pos = JSEditPrivate_ref(matchPositions).at(i);
            QTextEdit::ExtraSelection matchHighlight;
            matchHighlight.format.setBackground(JSEditPrivate_ref(bracketMatchColor));
            matchHighlight.cursor = textCursor();
            matchHighlight.cursor.setPosition(pos);
            matchHighlight.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
            extraSelections.append(matchHighlight);
        }

        for (int i = 0; i < JSEditPrivate_ref(errorPositions).count(); ++i) {
            int pos = JSEditPrivate_ref(errorPositions).at(i);
            QTextEdit::ExtraSelection errorHighlight;
            errorHighlight.format.setBackground(JSEditPrivate_ref(bracketErrorColor));
            errorHighlight.cursor = textCursor();
            errorHighlight.cursor.setPosition(pos);
            errorHighlight.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
            extraSelections.append(errorHighlight);
        }

        setExtraSelections(extraSelections);
    }
}

void
SCodeEditor::updateSidebar ( const QRect &rect, int d ) {
    Q_UNUSED(rect)
    if (d != 0)
        updateSidebar();
}

void SCodeEditor::insertCompletion(const QString &completion)
{
    if (m_pCompleter->widget() != this)
         return;
     QTextCursor tc = textCursor();
     int extra = completion.length() - m_pCompleter->completionPrefix().length();
     tc.movePosition(QTextCursor::Left);
     tc.movePosition(QTextCursor::EndOfWord);
     tc.insertText(completion.right(extra));
     setTextCursor(tc);

}

void
SCodeEditor::updateSidebar ( ) {
    if (!JSEditPrivate_ref(showLineNumbers) && !JSEditPrivate_ref(codeFolding)) {
        JSEditPrivate_ref(sidebar)->hide();
        setViewportMargins(0, 0, 0, 0);
        JSEditPrivate_ref(sidebar)->setGeometry(3, 0, 0, height());
        return;
    }

    JSEditPrivate_ref(sidebar)->foldIndicatorWidth = 0;
    JSEditPrivate_ref(sidebar)->font = this->font();
    JSEditPrivate_ref(sidebar)->show();

    int sw = 0;
    if (JSEditPrivate_ref(showLineNumbers)) {
        int digits = 2;
        int maxLines = blockCount();
        for (int number = 10; number < maxLines; number *= 10)
            ++digits;
        sw += fontMetrics().width('w') * digits;
    }
    if (JSEditPrivate_ref(codeFolding)) {
        int fh = fontMetrics().lineSpacing();
        int fw = fontMetrics().width('w');
        JSEditPrivate_ref(sidebar)->foldIndicatorWidth = qMax(fw, fh);
        sw += JSEditPrivate_ref(sidebar)->foldIndicatorWidth;
    }
    setViewportMargins(sw, 0, 0, 0);

    JSEditPrivate_ref(sidebar)->setGeometry(0, 0, sw, height());
    QRectF sidebarRect(0, 0, sw, height());

    QTextBlock block = firstVisibleBlock();
    int index = 0;
    while (block.isValid()) {
        if (block.isVisible()) {
            QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
            if (sidebarRect.intersects(rect)) {
                if (JSEditPrivate_ref(sidebar)->lineNumbers.count() >= index)
                    JSEditPrivate_ref(sidebar)->lineNumbers.resize(index + 1);
                JSEditPrivate_ref(sidebar)->lineNumbers[index].position = rect.top();
                JSEditPrivate_ref(sidebar)->lineNumbers[index].number = block.blockNumber() + 1;
                JSEditPrivate_ref(sidebar)->lineNumbers[index].foldable =
                  JSEditPrivate_ref(codeFolding) ? isFoldable(block.blockNumber() + 1) : false;
                JSEditPrivate_ref(sidebar)->lineNumbers[index].folded =
                  JSEditPrivate_ref(codeFolding) ? isFolded(block.blockNumber() + 1) : false;
                ++index;
            }
            if (rect.top() > sidebarRect.bottom())
                break;
        }
        block = block.next();
    }
    JSEditPrivate_ref(sidebar)->lineNumbers.resize(index);
    JSEditPrivate_ref(sidebar)->update();
}

void
SCodeEditor::mark ( const QString &str, Qt::CaseSensitivity sens ) {
    JSEditPrivate_ref(highlighter)->mark(str, sens);
}
