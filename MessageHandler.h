#pragma once

#include <QAbstractMessageHandler>

class MessageHandler : public QAbstractMessageHandler
{
public:
    MessageHandler() : QAbstractMessageHandler(nullptr)
    {
    }

    QString statusMessage() const
    {
        return m_description;
    }

    int line() const
    {
        return static_cast<int>(m_sourceLocation.line());
    }

    int column() const
    {
        return static_cast<int>(m_sourceLocation.column());
    }

    ~MessageHandler() override;
protected:
    virtual void handleMessage(QtMsgType type, const QString& description,
                               const QUrl& identifier, const QSourceLocation& sourceLocation) override
    {
        Q_UNUSED(type);
        Q_UNUSED(identifier);

        m_description = description;
        m_sourceLocation = sourceLocation;
    }
private:
    QString m_description;
    QSourceLocation m_sourceLocation;
};

MessageHandler::~MessageHandler()
{
}
