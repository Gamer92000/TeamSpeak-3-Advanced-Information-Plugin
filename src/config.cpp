#include "config.h"
#include "ui_config.h"
#include "sharedHeader.h"

config::config(const QString &configLocation, QWidget *parent /* = nullptr */) : QDialog(parent),
																																								 m_ui(std::make_unique<Ui::configui>()),
																																								 m_settings(std::make_unique<QSettings>(configLocation, QSettings::IniFormat, this))
{
	m_ui->setupUi(this);

	setWindowTitle("Advanced Information :: Config");

	// Connect UI Elements.
	connect(m_ui->ok_btn, &QPushButton::clicked, this, [&]()
					{
		saveSettings();
		close(); });
	connect(m_ui->cancel_btn, &QPushButton::clicked, this, &QWidget::close);
	connect(m_ui->apply_btn, &QPushButton::clicked, this, &config::saveSettings);
	connect(m_ui->reset_btn, &QPushButton::clicked, this, [&]()
					{
		m_settings->clear();
		loadSettings();
		resetConf(); });

	connect(m_ui->clientBox, &QCheckBox::toggled, this, [&](bool checked)
					{ m_ui->clientList->setEnabled(checked); });
	connect(m_ui->channelBox, &QCheckBox::toggled, this, [&](bool checked)
					{ m_ui->channelList->setEnabled(checked); });
	connect(m_ui->serverBox, &QCheckBox::toggled, this, [&](bool checked)
					{ m_ui->serverList->setEnabled(checked); });

	adjustSize();
	this->setSizeGripEnabled(false);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

config::~config()
{
	m_settings->sync();
}

void config::setConfigOption(const QString &option, const QVariant &value)
{
	m_settings->setValue(option, value);
}

QVariant config::getConfigOption(const QString &option) const
{
	return m_settings->value(option);
}

void config::showEvent(QShowEvent * /* e */)
{
	adjustSize();
	loadSettings();
}

void config::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::StyleChange && isVisible())
	{
		m_ui->verticalLayoutWidget_2->adjustSize();
		adjustSize();
	}
}

void config::saveSettings()
{
	for (int i = 0; i < m_ui->clientList->count(); ++i)
	{
		QListWidgetItem *item = m_ui->clientList->item(i);
		QString confName = "client_" + item->text();
		setConfigOption(confName, item->checkState());

		QString confPosName = "client_" + QString(i);
		setConfigOption(confPosName, item->text());
	}
	for (int i = 0; i < m_ui->channelList->count(); ++i)
	{
		QListWidgetItem *item = m_ui->channelList->item(i);
		QString confName = "channel_" + item->text();
		setConfigOption(confName, item->checkState());
		QString confPosName = "channel_" + QString(i);
		setConfigOption(confPosName, item->text());
	}
	for (int i = 0; i < m_ui->serverList->count(); ++i)
	{
		QListWidgetItem *item = m_ui->serverList->item(i);
		QString confName = "server_" + item->text();
		setConfigOption(confName, item->checkState());
		QString confPosName = "server_" + QString(i);
		setConfigOption(confPosName, item->text());
	}
	setConfigOption("client_enabled", m_ui->clientBox->isChecked());
	setConfigOption("channel_enabled", m_ui->channelBox->isChecked());
	setConfigOption("server_enabled", m_ui->serverBox->isChecked());
}

void config::loadSettings()
{
	int clientAmount = m_ui->clientList->count();
	for (int i = 0; i < clientAmount; ++i)
	{
		QListWidgetItem *item = m_ui->clientList->item(i);
		QString confName = "client_" + item->text();
		item->setCheckState((Qt::CheckState)getConfigOption(confName).toInt());
	}
	for (int i = 0; i < clientAmount; i++)
	{
		QString confPosName = "client_" + QString(i);
		if (getConfigOption(confPosName).isNull())
			continue;
		QString val = getConfigOption(confPosName).toString();
		QListWidgetItem *item = m_ui->clientList->takeItem(m_ui->clientList->row(m_ui->clientList->findItems(val, Qt::MatchExactly)[0]));
		m_ui->clientList->insertItem(i, item);
	}

	int channelAmount = m_ui->channelList->count();
	for (int i = 0; i < channelAmount; ++i)
	{
		QListWidgetItem *item = m_ui->channelList->item(i);
		QString confName = "channel_" + item->text();
		item->setCheckState((Qt::CheckState)getConfigOption(confName).toInt());
	}
	for (int i = 0; i < channelAmount; i++)
	{
		QString confPosName = "channel_" + QString(i);
		if (getConfigOption(confPosName).isNull())
			continue;
		QString val = getConfigOption(confPosName).toString();
		QListWidgetItem *item = m_ui->channelList->takeItem(m_ui->channelList->row(m_ui->channelList->findItems(val, Qt::MatchExactly)[0]));
		m_ui->channelList->insertItem(i, item);
	}

	int serverAmount = m_ui->serverList->count();
	for (int i = 0; i < serverAmount; ++i)
	{
		QListWidgetItem *item = m_ui->serverList->item(i);
		QString confName = "server_" + item->text();
		item->setCheckState((Qt::CheckState)getConfigOption(confName).toInt());
	}
	for (int i = 0; i < clientAmount; i++)
	{
		QString confPosName = "server_" + QString(i);
		if (getConfigOption(confPosName).isNull())
			continue;
		QString val = getConfigOption(confPosName).toString();
		QListWidgetItem *item = m_ui->serverList->takeItem(m_ui->serverList->row(m_ui->serverList->findItems(val, Qt::MatchExactly)[0]));
		m_ui->serverList->insertItem(i, item);
	}

	m_ui->clientBox->setChecked(getConfigOption("client_enabled").toBool());
	m_ui->channelBox->setChecked(getConfigOption("channel_enabled").toBool());
	m_ui->serverBox->setChecked(getConfigOption("server_enabled").toBool());

	m_ui->serverList->setEnabled(m_ui->serverBox->isChecked());
	m_ui->clientList->setEnabled(m_ui->clientBox->isChecked());
	m_ui->channelList->setEnabled(m_ui->channelBox->isChecked());
}