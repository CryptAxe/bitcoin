// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/sidechainpage.h>
#include <qt/forms/ui_sidechainpage.h>

#include <qt/bitcoinunits.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/walletmodel.h>

#include <base58.h>
#include <coins.h>
#include <consensus/validation.h>
#include <core_io.h>
#include <init.h>
#include <miner.h>
#include <net.h>
#include <primitives/block.h>
#include <sidechain.h>
#include <txdb.h>
#include <validation.h>
#include <wallet/coincontrol.h>
#include <wallet/wallet.h>

#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QStackedWidget>

#include <sstream>

SidechainPage::SidechainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SidechainPage)
{
    ui->setupUi(this);

    ui->listWidgetSidechains->setIconSize(QSize(32, 32));

    // Setup Sidechains list widget
    for (const Sidechain& s : ValidSidechains) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetSidechains);

        // Set icon
        QIcon icon(":/icons/bitcoin");
        item->setIcon(icon);

        // Set text
        item->setText(QString::fromStdString(s.GetSidechainName()));
        QFont font = item->font();
        font.setPointSize(22);
        item->setFont(font);

        ui->listWidgetSidechains->addItem(item);
    }

    ui->listWidgetSidechains->setCurrentRow(0);
}

SidechainPage::~SidechainPage()
{
    delete ui;
}

void SidechainPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if (model && model->getOptionsModel())
    {
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this,
                SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));
    }
}

void SidechainPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance,
                               const CAmount& immatureBalance, const CAmount& watchOnlyBalance,
                               const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance)
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    const CAmount& pending = immatureBalance + unconfirmedBalance;
    //ui->available->setText(BitcoinUnits::formatWithUnit(unit, balance, false, BitcoinUnits::separatorAlways));
    //ui->pending->setText(BitcoinUnits::formatWithUnit(unit, pending, false, BitcoinUnits::separatorAlways));
}
