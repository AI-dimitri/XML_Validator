#pragma once

#include <QtWidgets>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

class MyValidator : public QWidget
{
    Q_OBJECT
public:
    MyValidator(QWidget* pwgt = nullptr);
private:
    QXmlSchema schema;
    bool isSchema;
    QLabel* schemaValid;
    void xsdValidityChanged();

    QStringListModel* model;

    QStringList xmlNames;
    QTextEdit* xmlFile;
    QLabel* validationStatus;
    void moveCursor(int line, int column);
signals:
    void pathChanged(const QString&);
private slots:
    void xsdButtonClicked();
    void xmlButtonClicked();
    void folderButtonClicked();

    void fileSelected(const QModelIndex& index);

    void validate();
};
