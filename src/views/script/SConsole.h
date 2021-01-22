#ifndef SCONSOLE_H
#define SCONSOLE_H

#include <QPlainTextEdit>
#include <QTimer>

class SConsole : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit SConsole(QWidget *parent = nullptr);
    virtual ~SConsole();

    void addText(const QString &a_rstrText);

    void clearText();

private slots:
    void refresh();

private:
    QString m_strText;
    QTimer *m_pTimer;
};

#endif // SCONSOLE_H
