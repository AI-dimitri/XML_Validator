#include "MyValdator.h"
#include "XmlSyntaxHighlighter.h"
#include "MessageHandler.h"

MyValidator::MyValidator(QWidget* pwgt) : QWidget(pwgt), isSchema(false)
{    
    QLabel* xsdButtonText = new QLabel("Select XSD file:");
    QLabel* xsdPathText = new QLabel("Path to selected XSD file:");
    QLabel* xsdPath = new QLabel("Not selected");
    QLabel* schemaValidText = new QLabel("Validity of your XSD file:");
    QLabel* nameListText = new QLabel("Selected XMLfiles:");
    QLabel* xmlFileText = new QLabel("The XML file data:");
    schemaValid = new QLabel;
    validationStatus = new QLabel;

    QPushButton* xsdButton = new QPushButton("Select file");
    QPushButton* nameListButton = new QPushButton("Select files");
    QPushButton* nameFolderButton = new QPushButton("Select folder");
    QPushButton* validateButton = new QPushButton("Validate");


    model = new QStringListModel;
    QListView* pListView = new QListView;
    pListView->setModel(model);
    pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    xmlFile = new QTextEdit;
    xmlFile->setReadOnly(true);

    new XmlSyntaxHighlighter(xmlFile->document());

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(xsdButtonText, 0, 0, 1, 2);
    layout->addWidget(xsdPathText, 0, 2);
    layout->addWidget(schemaValidText, 0, 3);
    layout->addWidget(xsdButton, 1, 0, 1, 2);
    layout->addWidget(xsdPath, 1, 2);
    layout->addWidget(schemaValid, 1, 3);
    layout->addWidget(nameListText, 2, 0, 1, 2);
    layout->addWidget(xmlFileText, 2, 2, 1, 2);
    layout->addWidget(pListView, 3, 0, 5, 2);
    layout->addWidget(xmlFile, 3, 2, 5, 2);
    layout->addWidget(nameListButton, 8, 0);
    layout->addWidget(nameFolderButton, 8, 1);
    layout->addWidget(validationStatus, 8, 2);
    layout->addWidget(validateButton, 8, 3);

    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 10);
    layout->setColumnStretch(3, 10);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 0);
    layout->setRowStretch(2, 0);
    layout->setRowStretch(7, 10);
    layout->setRowStretch(8, 0);

    QObject::connect(xsdButton, SIGNAL(clicked()),
                     this, SLOT(xsdButtonClicked()));
    QObject::connect(this, SIGNAL(pathChanged(const QString&)),
                     xsdPath, SLOT(setText(const QString&)));
    QObject::connect(nameListButton, SIGNAL(clicked()),
                     this, SLOT(xmlButtonClicked()));
    QObject::connect(nameFolderButton, SIGNAL(clicked()),
                     this, SLOT(folderButtonClicked()));
    QObject::connect(pListView, SIGNAL(clicked(const QModelIndex&)),
                     this, SLOT(fileSelected(const QModelIndex&)));
    QObject::connect(validateButton, SIGNAL(clicked()),
                     this, SLOT(validate()));

    schemaValid->setText("Not selected");
    const QString styleSheet = QString("QLabel {background: %1; padding: 3px}").arg(QColor(Qt::yellow).lighter(160).name());
    schemaValid->setStyleSheet(styleSheet);
    validationStatus->setText("File is not selected");
    const QString xmlStyleSheet = QString("QLabel {background: %1; padding: 3px}").arg(QColor(Qt::yellow).lighter(160).name());
    validationStatus->setStyleSheet(xmlStyleSheet);

    setLayout(layout);
    setMinimumHeight(600);
    setMinimumWidth(1000);
}

void MyValidator::xsdButtonClicked()
{
    const QString newPath = QFileDialog::getOpenFileName(this, "Select the XSD file", QDir::homePath(), "XSD files (*.xsd)");
    if (newPath != "")
    {
        QFile file(newPath.toUtf8().constData());
        file.open(QIODevice::ReadOnly);

        emit pathChanged(newPath);

        bool validity;
        schema.load(&file, QUrl::fromLocalFile(file.fileName()));
        !schema.isValid() ? validity = false : validity = true;
        if (validity != isSchema)
        {
            isSchema = validity;
            xsdValidityChanged();
        }
    }
}

void MyValidator::xsdValidityChanged()
{
    QString name;
    if (isSchema)
        name = "Valid";
    else
        name = "Not valid";

    schemaValid->setText(name);
    const QString styleSheet = QString("QLabel {background: %1; padding: 3px}").arg(isSchema ? QColor(Qt::green).lighter(160).name() : QColor(Qt::red).lighter(160).name());
    schemaValid->setStyleSheet(styleSheet);
}

void MyValidator::xmlButtonClicked()
{
    QStringList xmlNamesTemp = QFileDialog::getOpenFileNames(this, "Select the XML files", QDir::homePath(), "XML files (*.xml)");
    if (xmlNamesTemp.size() != 0)
    {
        xmlNames = xmlNamesTemp;
        model->removeRows(0, model->rowCount());
        xmlFile->clear();
        model->setStringList(xmlNames);
    }
}

void MyValidator::folderButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select the folder", QDir::homePath(), QFileDialog::ShowDirsOnly);
    QDir dir;
    QStringList file_names("*.xml");
    dir.setFilter(QDir::Files);
    dir.setNameFilters(file_names);
    file_names.clear();
    dir.setPath(folderPath);

    QFileInfoList list = dir.entryInfoList();
    QString name;
    for (QFileInfo finfo : list)
    {
        name = finfo.absoluteFilePath();
        file_names.push_back(name);
    }

    if (folderPath != "")
    {
        model->removeRows(0, model->rowCount());
        xmlFile->clear();
        if (file_names.size() != 0)
        {
            model->setStringList(file_names);
        }
    }
}

void MyValidator::fileSelected(const QModelIndex& index)
{
    const QString name = model->data(index).toString();
    QFile file(name.toUtf8().constData());
    file.open(QIODevice::ReadOnly);

    const QString text(QString::fromUtf8(file.readAll()));
    xmlFile->clear();
    xmlFile->setPlainText(text);

    validationStatus->setText("File is not validated");
    const QString styleSheet = QString("QLabel {background: %1; padding: 3px}").arg(QColor(Qt::yellow).lighter(160).name());
    validationStatus->setStyleSheet(styleSheet);
}

void MyValidator::validate()
{
    if ((model->rowCount() == 0 || xmlFile->toPlainText().toUtf8() == "") && !isSchema)
        QMessageBox::warning(this, "Error", "Your XSD file is not valid and you are not selected XML file.");
    if ((model->rowCount() != 0 && xmlFile->toPlainText().toUtf8() != "") && !isSchema)
        QMessageBox::warning(this, "Error", "Your XSD file is not valid.");
    if ((model->rowCount() == 0 || xmlFile->toPlainText().toUtf8() == "") && isSchema)
        QMessageBox::warning(this, "Error", "You are not selected XML file.");
    if ((model->rowCount() != 0 && xmlFile->toPlainText().toUtf8() != "") && isSchema)
    {
        const QByteArray xmlData = xmlFile->toPlainText().toUtf8();

        MessageHandler messageHandler;
        schema.setMessageHandler(&messageHandler);

        QXmlSchemaValidator validator(schema);
        bool error = false;
        if (!validator.validate(xmlData))
            error = true;

        if (error) {
            validationStatus->setText(messageHandler.statusMessage());
            moveCursor(messageHandler.line(), messageHandler.column());
        }
        else
            validationStatus->setText("Validation successful.");

        const QString styleSheet = QString("QLabel {background: %1; padding: 3px}").arg(error ? QColor(Qt::red).lighter(160).name() : QColor(Qt::green).lighter(160).name());
        validationStatus->setStyleSheet(styleSheet);
    }
}

void MyValidator::moveCursor(int line, int column)
{
    xmlFile->moveCursor(QTextCursor::Start);
    for (int i = 1; i < line; ++i)
        xmlFile->moveCursor(QTextCursor::Down);

    for (int i = 1; i < column; ++i)
        xmlFile->moveCursor(QTextCursor::Right);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    const QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = xmlFile->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    xmlFile->setExtraSelections(extraSelections);

    xmlFile->setFocus();
}
